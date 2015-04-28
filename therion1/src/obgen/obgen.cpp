
#include "core/io/output.hpp"
#include "core/core/pager.hpp"
#include "core/core/list.hpp"
#include "obgen/db.hpp"
#include "obgen/obgen.hpp"

#include "obgen/cs.hpp"
#include "core/io/file.hpp"
#include "core/env/time.hpp"
#include "core/env/console.hpp"
#include "core/core/seq.hpp"
#include "core/core/hashmap.hpp"

class TypeSeq
{
public:
	static bool contain(obgen::TypeId type_id)
	{
		static TypeSeq type_seq;
		return core::uint32_nil != type_seq.the_seq.search(obgen::cast(type_id));
	}
private:
	TypeSeq()
	{
		static const core::String types[] = {
			"String",
			"int8", "int16", "int32", "int64",
			"uint8", "uint16", "uint32", "uint64",
			"float32", "float64", "float128" };
		core::uint32 size = sizeof(types) / sizeof(core::String);
		the_seq.capacity(size);
		for(core::uint32 i = 0; i < size; i++)
			the_seq.append(types[i]);
		the_seq.sort();
	}
private:
	core::Seq<core::String> the_seq;
};

class Build
{
public:
	static void obgen2db(core::String& obgen_filename, core::String& db_filename)
	{
		io::File db_file(db_filename);
		db_file.open(io::File::write_only, true);

		io::File obgen_file(obgen_filename);
		obgen_file.open(io::File::read_only);

		// read obgen file and add closing 0
		core::uint32 file_size = obgen_file.size();
		core::String obgen_content;
		obgen_content.size(1 + file_size); // 1 + for closing 0
		io::Decode io_decode(obgen_file.input());
		io_decode.read(obgen_content.data(), file_size);
		obgen_content.at(file_size) = 0;

		// parse
		obgen::Decode decode;
		core::String::Iterator it = obgen_content.head();
		while(it.next() && decode.put(it.value()));
		obgen::Model model = decode.model();
		if(!model)
			env::Fail(cs_syntax_error).arg(obgen_filename).arg(decode.line_cnt()).fire();

		Build build(obgen_filename, db_file.output());
		build.db(obgen_file.core(), model);

		db_file.close();
	}
private:
	struct GlobalContext
	{
		GlobalContext() : clazz_seq(0), base_clazz(0) {}

		db::ClazzSeq clazz_seq;
		obgen::Clazz base_clazz;
	};
private:
	Build(core::String& obgen_filename, io::Output& output)
		: the_obgen_filename(obgen_filename), the_encode(output), the_clazz_map(0x9), the_global_map(0x9)
	{

	}
	void db(core::String& file_guard, obgen::Model model)
	{
		// struct, encode, decode
		db::StructPlus struct_plus = the_build.acquire_StructPlus();
		db::FuncComparePlus func_compare_plus = the_build.acquire_FuncComparePlus();
		db::FuncSizePlus func_size_plus = the_build.acquire_FuncSizePlus();
		db::FuncShiftPlus func_shift_plus = the_build.acquire_FuncShiftPlus();
		db::FuncWritePlus func_write_plus = the_build.acquire_FuncWritePlus();
		db::FuncReadPlus func_read_plus = the_build.acquire_FuncReadPlus();

		core::List<obgen::Clazz>::Iterator clazz_it = model->clazz_plus->head();
		while(clazz_it.next())
		{
			obgen::Clazz clazz = clazz_it.value();
			the_clazz_map.at(obgen::cast(clazz->clazz_id)) = clazz;

			db::ClazzVarId clazz_var_id = the_build.type_to_var(clazz->clazz_id);

			acquire_struct(struct_plus, clazz);
			acquire_func_compare(func_compare_plus, clazz, clazz_var_id);
			acquire_func_size(func_size_plus, clazz, clazz_var_id);
			acquire_func_shift(func_shift_plus, clazz, clazz_var_id);
			acquire_func_write(func_write_plus, clazz, clazz_var_id);
			acquire_func_read(func_read_plus, clazz, clazz_var_id);
		}

		db::ClazzIndex clazz_index = the_build.acquire_ClazzIndex(func_compare_plus);
		db::ClazzUtil clazz_util = the_build.acquire_ClazzUtil(func_size_plus, func_shift_plus);
		db::ClazzEncode clazz_encode = the_build.acquire_ClazzEncode(func_write_plus);
		db::ClazzDecode clazz_decode = the_build.acquire_ClazzDecode(func_read_plus);

		// persist
		db::ClazzSeqPlus clazz_seq_plus = the_build.acquire_ClazzSeqPlus();
		db::CallOpenPlus call_open_plus = the_build.acquire_CallOpenPlus();
		db::CallClosePlus call_close_plus = the_build.acquire_CallClosePlus();
		db::FuncGetSeqPlus func_get_seq_plus = the_build.acquire_FuncGetSeqPlus();
		db::FieldSeqPlus field_seq_plus = the_build.acquire_FieldSeqPlus();

		core::List<obgen::Global>::Iterator global_it = model->global_plus->head();
		while(global_it.next())
		{
			obgen::Global global = global_it.value();
			if(global->type_id)
			{
				core::String global_base_type_id = obgen::cast(global->type_id);
				db::TypeId global_type_id = the_build.var_to_type(global->var_id);
				core::String global_var_id = obgen::cast(global->var_id);

				// clazz seq
				db::ClazzSeq clazz_seq = the_build.acquire_ClazzSeq(global_type_id, global->type_id, global->var_id, 0, 0, 0, 0, 0, 0, 0);
				clazz_seq_plus->append(clazz_seq);

				GlobalContext& global_context = the_global_map.at(global_var_id);
				if(global_context.clazz_seq)
					env::Fail(cs_redefined_id).arg(the_obgen_filename).arg(global->line).arg(global_base_type_id).fire();

				core::Shared<obgen::Clazz> base_clazz = the_clazz_map.lookup(global_base_type_id);
				if(core::nil(base_clazz))
					env::Fail(cs_unknown_id).arg(the_obgen_filename).arg(global->line).arg(global_base_type_id).fire();

				global_context.clazz_seq = clazz_seq;
				global_context.base_clazz = base_clazz;

				// persist
				db::CallOpen call_open = the_build.acquire_CallOpen(global->var_id);
				call_open_plus->append(call_open);

				db::CallClose call_close = the_build.acquire_CallClose(global->var_id);
				call_close_plus->append(call_close);

				db::FuncGetSeq func_get_seq = the_build.acquire_FuncGetSeq(global_type_id, global->var_id);
				func_get_seq_plus->append(func_get_seq);

				db::FieldSeq field_seq = the_build.acquire_FieldSeq(global_type_id, global->var_id);
				field_seq_plus->append(field_seq);
			}
			if(global->index)
			{
				core::String global_var_id = obgen::cast(global->index->var_id);
				db::IndexId index_id = the_build.acquire_String();
				db::ShiftFieldPlus shift_field_plus = the_build.acquire_ShiftFieldPlus();
				db::CopyFieldPlus copy_field_plus = the_build.acquire_CopyFieldPlus();
				db::CompareFieldPlus compare_field_plus = the_build.acquire_CompareFieldPlus();

				core::Shared<GlobalContext> global_context = the_global_map.lookup(global_var_id);
				if(core::nil(global_context))
					env::Fail(cs_unknown_id).arg(the_obgen_filename).arg(global->line).arg(global_var_id).fire();

				core::List<obgen::Square>::Iterator square_it = global->index->square_plus->head();
				while(square_it.next())
				{
					obgen::Square square = square_it.value();
					bool result = acquire_index(index_id, shift_field_plus, copy_field_plus, compare_field_plus, square->var_chain, global_context->base_clazz);
					if(!result)
						env::Fail(cs_unknown_index)
							.arg(the_obgen_filename)
							.arg(global->line)
							.arg(global_var_id)
							.arg(obgen::cast(square))
							.fire();
				}

				db::ClazzSeq clazz_seq = global_context->clazz_seq;
				if(!clazz_seq->declare_result_ques) // first index to be added
				{
					clazz_seq->declare_result_ques = the_build.acquire_DeclareResult();
					clazz_seq->func_open_plus = the_build.acquire_FuncOpenPlus();
					clazz_seq->func_close_plus = the_build.acquire_FuncClosePlus();
					clazz_seq->func_find_all_plus = the_build.acquire_FuncFindAllPlus();
					clazz_seq->private_section_ques = the_build.acquire_PrivateSection();
					clazz_seq->func_index_plus = the_build.acquire_FuncIndexPlus();
					clazz_seq->field_index_plus = the_build.acquire_FieldIndexPlus();
				}

				db::FuncOpen func_open = the_build.acquire_FuncOpen(index_id, global->index->var_id);
				db::FuncClose func_close = the_build.acquire_FuncClose(index_id);
				db::FuncFindAll func_find_all = the_build.acquire_FuncFindAll(global_context->base_clazz->clazz_id, index_id);
				db::FuncIndex func_index = the_build.acquire_FuncIndex(index_id, shift_field_plus, copy_field_plus, compare_field_plus);
				db::FieldIndex field_index = the_build.acquire_FieldIndex(index_id);

				clazz_seq->func_open_plus->append(func_open);
				clazz_seq->func_close_plus->append(func_close);
				clazz_seq->func_find_all_plus->append(func_find_all);
				clazz_seq->func_index_plus->append(func_index);
				clazz_seq->field_index_plus->append(field_index);
			}
		}

		db::ClazzIterator clazz_iterator = the_build.acquire_ClazzIterator();
		db::ClazzPersistSeq clazz_persist_seq = the_build.acquire_ClazzPersistSeq();
		db::Now now = the_build.acquire_String(env::Time::now());
		db::ClazzPersist clazz_persist = the_build.acquire_ClazzPersist(now,
																		call_open_plus,
																		call_close_plus,
																		func_get_seq_plus,
																		field_seq_plus);

		// model
		db::NameSpace name_space = the_build.acquire_String(file_guard);
		db::Model db_model = the_build.acquire_Model(name_space,
													struct_plus,
													clazz_index,
													clazz_util,
													clazz_encode,
													clazz_decode,
													clazz_iterator,
													clazz_persist_seq,
													clazz_seq_plus,
													clazz_persist);
		the_encode.write(db_model);
	}
	void acquire_struct(db::StructPlus struct_plus, obgen::Clazz obgen_clazz)
	{
		db::ClazzId clazz_id = obgen_clazz->clazz_id;
		db::FieldPlus field_plus = the_build.acquire_FieldPlus();
		if(obgen_clazz->field_plus)
		{
			core::List<obgen::Field>::Iterator obgen_field_it = obgen_clazz->field_plus->head();
			while(obgen_field_it.next())
			{
				obgen::Field obgen_field = obgen_field_it.value();
				if(obgen_field->type_id)
				{
					db::TypeId type_id = obgen_field->type_id;
					db::VarId var_id = obgen_field->var_id;
					db::Field field = 0;
					if(TypeSeq::contain(type_id))
						field = the_build.acquire_Field(type_id, var_id);
					else if(2 == obgen_field->reduction) // [ ]
						field = the_build.acquire_Field1(type_id, var_id);
					else
						field = the_build.acquire_Field2(type_id, var_id);
					field_plus->append(field);
				}
			}
		}
		db::Struct db_struct = the_build.acquire_Struct(clazz_id, field_plus);
		struct_plus->append(db_struct);
	}
	void acquire_func_compare(db::FuncComparePlus func_compare_plus, obgen::Clazz obgen_clazz, db::ClazzVarId clazz_var_id)
	{
		db::ClazzId clazz_id = obgen_clazz->clazz_id;
		db::CallComparePlus call_compare_plus = the_build.acquire_CallComparePlus();
		if(obgen_clazz->field_plus)
		{
			core::List<obgen::Field>::Iterator obgen_field_it = obgen_clazz->field_plus->head();
			while(obgen_field_it.next())
			{
				obgen::Field obgen_field = obgen_field_it.value();
				if(obgen_field->type_id)
				{
					db::TypeId type_id = obgen_field->type_id;
					db::VarId var_id = obgen_field->var_id;
					db::CallCompare call_compare = (TypeSeq::contain(type_id))
						? the_build.acquire_CallCompare(clazz_var_id, var_id)
						: the_build.acquire_CallCompare1(clazz_var_id, var_id);
					call_compare_plus->append(call_compare);
				}
			}
		}
		db::FuncCompare func_compare = the_build.acquire_FuncCompare(clazz_id, clazz_var_id, call_compare_plus);
		func_compare_plus->append(func_compare);
	}
	void acquire_func_size(db::FuncSizePlus func_size_plus, obgen::Clazz obgen_clazz, db::ClazzVarId clazz_var_id)
	{
		db::ClazzId clazz_id = obgen_clazz->clazz_id;
		db::CallSizePlus call_size_plus = the_build.acquire_CallSizePlus();
		if(obgen_clazz->field_plus)
		{
			core::List<obgen::Field>::Iterator obgen_field_it = obgen_clazz->field_plus->head();
			while(obgen_field_it.next())
			{
				obgen::Field obgen_field = obgen_field_it.value();
				if(obgen_field->type_id)
				{
					db::TypeId type_id = obgen_field->type_id;
					db::VarId var_id = obgen_field->var_id;
					db::CallSize call_size = 0;
					if(type_id->equal(cs_string_type))
						call_size = the_build.acquire_CallSize(clazz_var_id, var_id);
					else if(TypeSeq::contain(type_id))
						call_size = the_build.acquire_CallSize1(clazz_var_id, var_id);
					else
						call_size = the_build.acquire_CallSize2(clazz_var_id, var_id);
					call_size_plus->append(call_size);
				}
			}
		}
		db::FuncSize func_size = the_build.acquire_FuncSize(clazz_id, clazz_var_id, call_size_plus);
		func_size_plus->append(func_size);
	}
	void acquire_func_shift(db::FuncShiftPlus func_shift_plus, obgen::Clazz obgen_clazz, db::ClazzVarId clazz_var_id)
	{
		db::ClazzId clazz_id = obgen_clazz->clazz_id;
		db::CallShiftPlus call_shift_plus = the_build.acquire_CallShiftPlus();
		if(obgen_clazz->field_plus)
		{
			core::List<obgen::Field>::Iterator obgen_field_it = obgen_clazz->field_plus->head();
			while(obgen_field_it.next())
			{
				obgen::Field obgen_field = obgen_field_it.value();
				if(obgen_field->type_id)
				{
					db::TypeId type_id = obgen_field->type_id;
					db::CallShift call_shift = 0;
					if(type_id->equal(cs_string_type))
						call_shift = the_build.acquire_CallShift();
					else if(TypeSeq::contain(type_id))
						call_shift = the_build.acquire_CallShift1(type_id);
					else if(2 == obgen_field->reduction) // [ ]
						call_shift = the_build.acquire_CallShift2(type_id);
					else
						call_shift = the_build.acquire_CallShift3(type_id);
					call_shift_plus->append(call_shift);
				}
			}
		}
		db::FuncShift func_shift = the_build.acquire_FuncShift(clazz_id, call_shift_plus);
		func_shift_plus->append(func_shift);
	}
	void acquire_func_write(db::FuncWritePlus func_write_plus, obgen::Clazz obgen_clazz, db::ClazzVarId clazz_var_id)
	{
		db::ClazzId clazz_id = obgen_clazz->clazz_id;
		db::CallWritePlus call_write_plus = the_build.acquire_CallWritePlus();
		if(obgen_clazz->field_plus)
		{
			core::List<obgen::Field>::Iterator obgen_field_it = obgen_clazz->field_plus->head();
			while(obgen_field_it.next())
			{
				obgen::Field obgen_field = obgen_field_it.value();
				if(obgen_field->type_id)
				{
					db::TypeId type_id = obgen_field->type_id;
					db::VarId var_id = obgen_field->var_id;
					db::CallWrite call_write = 0;
					if(type_id->equal(cs_string_type))
						call_write = the_build.acquire_CallWrite(clazz_var_id, var_id);
					else if(TypeSeq::contain(type_id))
						call_write = the_build.acquire_CallWrite1(clazz_var_id, var_id);
					else
						call_write = the_build.acquire_CallWrite2(clazz_var_id, var_id);
					call_write_plus->append(call_write);
				}
			}
		}
		db::FuncWrite func_write = the_build.acquire_FuncWrite(clazz_id, clazz_var_id, call_write_plus);
		func_write_plus->append(func_write);
	}
	void acquire_func_read(db::FuncReadPlus func_read_plus, obgen::Clazz obgen_clazz, db::ClazzVarId clazz_var_id)
	{
		db::ClazzId clazz_id = obgen_clazz->clazz_id;
		db::CallReadPlus call_read_plus = the_build.acquire_CallReadPlus();
		if(obgen_clazz->field_plus)
		{
			core::List<obgen::Field>::Iterator obgen_field_it = obgen_clazz->field_plus->head();
			while(obgen_field_it.next())
			{
				obgen::Field obgen_field = obgen_field_it.value();
				if(obgen_field->type_id)
				{
					db::TypeId type_id = obgen_field->type_id;
					db::VarId var_id = obgen_field->var_id;
					db::CallRead call_read = 0;
					if(type_id->equal(cs_string_type))
						call_read = the_build.acquire_CallRead(clazz_var_id, var_id);
					else if(TypeSeq::contain(type_id))
						call_read = the_build.acquire_CallRead1(clazz_var_id, var_id);
					else
						call_read = the_build.acquire_CallRead2(clazz_var_id, var_id);
					call_read_plus->append(call_read);
				}
			}
		}
		db::FuncRead func_read = the_build.acquire_FuncRead(clazz_id, clazz_var_id, call_read_plus);
		func_read_plus->append(func_read);
	}
	bool acquire_index(db::IndexId index_id,
		db::ShiftFieldPlus shift_field_plus, db::CopyFieldPlus copy_field_plus, db::CompareFieldPlus compare_field_plus,
		obgen::VarChain var_chain, obgen::Clazz base_clazz)
	{
		db::CallShiftPlus call_shift_plus = the_build.acquire_CallShiftPlus();

		core::List<db::VarId>::Iterator var_id_it = var_chain->head();
		while(var_id_it.next())
		{
			db::VarId db_var_id = var_id_it.value();
			core::String var_id = obgen::cast(db_var_id);

			// index_id
			index_id->attach('_');
			index_id->append(var_id);

			// shift_field_plus, copy_field_plus, compare_field_plus
			core::List<obgen::Field>::Iterator field_it = base_clazz->field_plus->head();
			while(field_it.next())
			{
				obgen::Field obgen_field = field_it.value();
				db::TypeId type_id = obgen_field->type_id;
				if(obgen_field->var_id->equal(var_id))
				{
					if(type_id->equal(cs_string_type))
					{
						db::ShiftField shift_field = the_build.acquire_ShiftField(call_shift_plus);
						shift_field_plus->append(shift_field);
						db::CopyField copy_field = the_build.acquire_CopyField();
						copy_field_plus->append(copy_field);
						db::CompareField compare_field = the_build.acquire_CompareField();
						compare_field_plus->append(compare_field);
						return !var_id_it.next();
					}
					else if(TypeSeq::contain(type_id))
					{
						db::ShiftField shift_field = the_build.acquire_ShiftField1(call_shift_plus, type_id);
						shift_field_plus->append(shift_field);
						db::CopyField copy_field = the_build.acquire_CopyField1(type_id);
						copy_field_plus->append(copy_field);
						db::CompareField compare_field = the_build.acquire_CompareField1(type_id);
						compare_field_plus->append(compare_field);
						return !var_id_it.next();
					}
					else
					{
						core::Shared<obgen::Clazz> base_clazz_share = the_clazz_map.lookup(obgen::cast(type_id));
						if(core::nil(base_clazz_share))
							return false;
						base_clazz = base_clazz_share;
					}
					break;
				}
				else
				{
					db::CallShift call_shift = 0;
					if(type_id->equal(cs_string_type))
						call_shift = the_build.acquire_CallShift();
					else if(TypeSeq::contain(type_id))
						call_shift = the_build.acquire_CallShift1(type_id);
					else if(2 == obgen_field->reduction) // [ ]
						call_shift = the_build.acquire_CallShift2(type_id);
					else
						call_shift = the_build.acquire_CallShift3(type_id);
					call_shift_plus->append(call_shift);
				}
			}
			if(field_it.tail())
				return false;
		}
		return false;
	}
private:
	core::String the_obgen_filename;
	db::Encode the_encode;
	db::Build the_build;
	core::HashMap<core::String, obgen::Clazz> the_clazz_map;
	core::HashMap<core::String, GlobalContext> the_global_map;
};

int main(int argc, char* argv[])
{
	env::init(argc, argv);
	try
	{
		bool verbose = false;
		core::String input_file;
		core::String output_file;

		for(core::int32 i = 1; i < argc; i++)
		{
			if(cs_verbose_switch.equal(argv[i]))
				verbose = true;
			else if(cs_input_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				input_file = argv[i];
			}
			else if(cs_output_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				output_file = argv[i];
			}
			else
				env::Fail::fire(cs_usage);
		}

		env::Log::verbose() = verbose;
		Build::obgen2db(input_file, output_file);
	}
	catch(env::Exception& exception)
	{
		env::Con::exception(exception);
	}
	catch(...)
	{
		env::Con::writeln(cs_unknown_exception);
	}
	return env::final();
}
