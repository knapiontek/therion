
#ifndef LANGEN_ENCODE_HPP
#define LANGEN_ENCODE_HPP

#include "langen/machine.hpp"
#include "langen/cs.hpp"
#include "core/io/file.hpp"

class UniqueEncode : public core::FormatClass<UniqueEncode>
{
public:
	UniqueEncode(io::Output& output) : the_output(output)
	{

	}
	UniqueEncode& format(core::String& format)
	{
		core::FormatClass<UniqueEncode>::format(format);
		return *this;
	}
	void write(core::String& arg)
	{
		the_output.output(arg.data(), arg.size());
	}
	void write()
	{
		core::String& arg = core::FormatClass<UniqueEncode>::end();
		the_output.output(arg.data(), arg.size());
	}
	void write(core::HashSet<core::String>& set)
	{
		core::String& arg = core::FormatClass<UniqueEncode>::end();
		if(set.put(arg))
			the_output.output(arg.data(), arg.size());
	}
private:
	io::Output& the_output;
};

class Encode
{
public:
	static void write(Model& model, core::String& filename)
	{
		io::File file(filename);
		file.open(io::File::write_only, true);

		Encode encode(model, file.output());
		encode.write(file.core());

		file.close();
	}
private:
	Encode(Model& model, io::Output& output)
		: the_model(model), the_encode(output), the_node_set(0xFF)
	{
		the_indent.data = cs_tabs.data();
		the_indent.size = 0;
	}
	void write(core::String& name)
	{
		the_encode.format(cs_file_begin).arg(name).write();

		if(Model::none != the_model.level)
		{
			write_machine();
			write_input_id();
		}

		the_encode.write(cs_basics);
		write_constants();
		write_forwards();
		write_typedefs();
		write_structs();

		the_encode.write(cs_clazz_merge);
		write_functions_append();
		the_encode.write(cs_clazz_encode);
		write_functions_write();

		the_encode.write(cs_clazz_build_begin);
		write_functions_acquire();
		the_encode.write(cs_clazz_build_end);

		if(Model::none != the_model.level)
		{
			the_encode.format(cs_clazz_decode_begin).arg(1 + (Model::verbose == the_model.level)).write();
			write_put();
			write_enum_product();
			write_enum_reduction();
			write_union_heap();
			the_encode.write(cs_clazz_decode_end);
		}

		the_encode.format(cs_file_end).arg(name).write();
	}
	void write_machine()
	{
		Machine machine(the_model);

		Machine::Node root;
		machine.create_tree(root);

		if(Model::verbose == the_model.level) // 2 dim, direct tree representation
		{
			// comments
			the_encode.write(cs_machine_comment_begin);
			core::HashMap<core::String, Model::Item>::Iterator it = the_model.item_map.head();
			while(it.next())
			{
				core::String& item_key = it.key();
				the_encode.format(cs_space_arg1).arg(Mangle::hex(item_key)).write();
			}
			the_encode.write(cs_machine_comment_line);
			write_machine_comment(root);
			the_node_set.erase_all();
			the_encode.write(cs_machine_comment_end);

			// state machine
			core::assert(-1 == the_model.product->id); // next line assumes it
			core::uint32 size = the_model.product_list.size() - 1 + the_model.input_map.size();
			the_encode.format(cs_machine_2d_begin).arg(size).write();
			write_machine_2d(root);
			the_node_set.erase_all();
			the_encode.write(cs_machine_2d_end);
		}
		else // 1 dim, packed state machine
		{
			Machine::Package package;
			machine.pack_tree(package, root);
			write_machine_1d(package);
		}
	}
	void write_machine_comment(Machine::Node::share node)
	{
		the_indent.size++;
		the_encode.format(cs_machine_comment_line_indent).arg(the_indent).write();

		write_node(node);
		while(node->follow_map.page_size() && 1 == node->follow_map.size()) // continue in-line
		{
			Machine::Node::FollowMap::Iterator it = node->follow_map.head();
			it.next();
			node = it.value();
			the_encode.write(cs_space);
			write_node(node);
		}
		if(node->follow_map.page_size())
		{
			Machine::Node::FollowMap::Iterator follow_it = node->follow_map.head();
			while(follow_it.next())
			{
				Machine::Node& follow = follow_it.value();
				if(the_node_set.put(follow))
					write_machine_comment(follow);
				else
				{
					the_indent.size++;
					the_encode.format(cs_machine_comment_line_indent).arg(the_indent).write();
					write_node(follow);
					the_indent.size--;
				}
			}
		}

		the_indent.size--;
	}
	void write_node(Machine::Node& node)
	{
		core::String signature = node.name;
		if(Machine::Node::conflict & node.info)
			signature.append(cs_machine_comment_conflict);
		if(Machine::Node::merge & node.info)
			signature.append(cs_machine_comment_merge);
		if(Machine::Node::shift & node.flags)
			signature.append(cs_machine_comment_shift);
		if(Machine::Node::reduce & node.flags)
			signature.append(cs_machine_comment_reduce).append(node.code);
		the_encode.write(signature);
	}
	void write_machine_1d(Machine::Package& package)
	{
		the_encode.format(cs_machine_1d_begin)
			.arg(package.checkset_width)
			.arg(package.checkset_mask)
			.arg(package.start_offset)
			.write(); // shift, see map
		bool comma = false;
		core::List<core::uint32>::Iterator code_it = package.list.head();
		while(code_it.next())
		{
			core::uint32 code = code_it.value();
			core::String separator = comma++ ? cs_comma_space_arg1 : cs_space_arg1;
			the_encode.format(separator).arg(code).write();
		}
		the_encode.write(cs_machine_1d_end);
	}
	void write_machine_2d(Machine::Node& node, bool nested = false)
	{
		if(node.follow_map.page_size() && node.follow_map.size())
		{
			core::String begin = nested ? cs_machine_2d_comma_state_begin : cs_machine_2d_state_begin;
			the_encode.format(begin).arg(node.code).write();

			bool comma = false;
			core::int32 last_input_id = -1;
			Machine::Node::FollowMap::Sort follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				core::uint32 input_id = follow_it.key();
				Machine::Node& follow = follow_it.value();
				core::uint32 i = input_id - last_input_id;
				while(--i)
					the_encode.write(comma++ ? cs_comma_zero : cs_zero);
				the_encode.format(comma++ ? cs_machine_2d_comma_cell : cs_machine_2d_cell)
					.arg(follow.code)
					.arg(follow.flags)
					.write();
				last_input_id = input_id;
			}

			the_encode.write(cs_machine_2d_state_end);

			follow_it = node.follow_map.sort();
			while(follow_it.next())
			{
				Machine::Node& follow = follow_it.value();
				if(the_node_set.put(follow))
					write_machine_2d(follow, true);
			}
		}
	}
	void write_input_id()
	{
		the_encode.write(cs_input_id_begin);

		bool comma = false;
		core::int32 last_byte = -1;
		core::TreeMap<core::uint8, Model::Input>::Sort input_it = the_model.input_map.sort();
		while(input_it.next())
		{
			core::uint8 byte = input_it.key();
			Model::Input& input = input_it.value();
			for(core::uint32 i = ++last_byte; i < byte; i++)
				the_encode.write(comma++ ? cs_comma_zero : cs_zero);
			the_encode.format(comma++ ? cs_comma_space_arg1 : core::String::nil()).arg(input.id).write();
			last_byte = byte;
		}

		the_encode.write(cs_input_id_end);
	}
	void write_constants()
	{
		core::List<Model::Item::share>::Iterator terminal_it = the_model.terminal_list.head();
		while(terminal_it.next())
		{
			Model::Item& terminal = terminal_it.value();
			if(1 == terminal.key.size() && terminal.key.at(0))
			{
				Model::Input& input = the_model.input_map.lookup(terminal.key.at(0));
				the_encode.format(cs_decl_known_byte)
					.arg(terminal.var)
					.arg(Mangle::value2constant(terminal.key))
					.arg(input.id)
					.write();
			}
			else
			{
				the_encode.format(cs_decl_known_string)
					.arg(terminal.var)
					.arg(Mangle::value2constant(terminal.key))
					.write();
				core::String::Iterator byte_it = terminal.key.head();
				while(byte_it.next())
				{
					core::uint8 byte = byte_it.value();
					Model::Input& input = the_model.input_map.at(byte);
					the_encode.format(cs_space_arg1).arg(input.id).write();
				}
			}
		}
	}
	void write_forwards()
	{
		the_encode.write(cs_line);
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			if(Model::Product::list == product.clazz
				|| Model::Product::complex == product.clazz
				|| Model::Product::abstract == product.clazz)
				the_encode.format(cs_struct_forward).arg(product.item->type).write();
		}
	}
	void write_typedefs()
	{
		the_encode.write(cs_line);
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			switch(product.clazz)
			{
			case Model::Product::list:
				the_encode.format(cs_typedef_list)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			case Model::Product::complex:
			case Model::Product::abstract:
				the_encode.format(cs_typedef_complex)
					.arg(product.item->type)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			case Model::Product::optional_list:
			case Model::Product::optional_complex:
			case Model::Product::optional_abstract:
				the_encode.format(cs_typedef_complex)
					.arg(product.reduction_seq.at(0).element_seq.at(0).item->type)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			case Model::Product::string:
			case Model::Product::fake:
				the_encode.format(cs_typedef_string)
					.arg(product.item->type)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			case Model::Product::optional_string:
			case Model::Product::optional_fake:
				the_encode.format(cs_typedef_string)
					.arg(product.reduction_seq.at(0).element_seq.at(0).item->type)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			case Model::Product::byte:
				the_encode.format(cs_typedef_byte)
					.arg(product.item->type)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			case Model::Product::optional_byte:
				the_encode.format(cs_typedef_byte)
					.arg(product.reduction_seq.at(0).element_seq.at(0).item->type)
					.arg(product.item->type)
					.arg(product.id)
					.write();
				break;
			default:
				core::assert(false);
			}
		}
	}
	void write_structs()
	{
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			if(Model::Product::list == product.clazz)
				the_encode.format(cs_struct_list)
					.arg(product.item->type)
					.arg(product.reduction_seq.at(1).element_seq.at(0).item->type)
					.write();
			else if(Model::Product::complex == product.clazz)
			{
				the_encode.format(1 == product.reduction_seq.size() ? cs_struct_begin : cs_struct_begin_reduction)
					.arg(product.item->type)
					.write();

				core::HashSet<core::String> decl_set(the_average_reduction_size);

				core::Seq<Model::Reduction>::Iterator reduction_it = product.reduction_seq.head();
				while(reduction_it.next())
				{
					Model::Reduction& reduction = reduction_it.value();
					core::Seq<Model::Element>::Iterator field_it = reduction.element_seq.head();
					while(field_it.next())
					{
						Model::Element& field = field_it.value();
						if(core::is(field.item->product)
							&& (!(Model::Product::fake & field.item->product->clazz) || (field.item->var == field.var)))
						{
							the_encode.format(cs_struct_field)
								.arg(field.item->type)
								.arg(field.var)
								.write(decl_set);
						}
					}
				}

				the_encode.write(cs_struct_end);
			}
		}
	}
	void write_functions_append()
	{
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			switch(product.clazz)
			{
			case Model::Product::list:
			{
				Model::Item& subitem = product.reduction_seq.at(1).element_seq.at(0).item;
				the_encode.format(cs_decl_append_list)
					.arg(product.item->type)
					.arg(product.item->var)
					.arg(subitem.type)
					.arg(subitem.var)
					.write();
				break;
			}
			case Model::Product::complex:
				write_function_append(product);
				break;
			case Model::Product::abstract:
				the_encode.format(cs_decl_append_abstract)
					.arg(product.item->type)
					.arg(product.item->var)
					.write();
				break;
			case Model::Product::string:
			case Model::Product::fake:
			case Model::Product::byte:
			case Model::Product::optional_list:
			case Model::Product::optional_complex:
			case Model::Product::optional_string:
			case Model::Product::optional_fake:
			case Model::Product::optional_byte:
			case Model::Product::optional_abstract:
				break;
			default:
				core::assert(false);
			}
		}
	}
	void write_function_append(Model::Product& product)
	{
		bool single = 1 == product.reduction_seq.size();
		the_encode.format(single ? cs_decl_append_begin : cs_decl_append_begin_switch)
			.arg(product.item->type)
			.arg(product.item->var)
			.write();

		core::Seq<Model::Reduction>::Iterator reduction_it = product.reduction_seq.head();
		while(reduction_it.next())
		{
			Model::Reduction& reduction = reduction_it.value();

			if(!single)
				the_encode.format(cs_call_append_case_switch).arg(reduction_it.position()).write();

			core::Seq<Model::Element>::Iterator field_it = reduction.element_seq.head();
			while(field_it.next())
			{
				Model::Element& field = field_it.value();
				if(core::is(field.item->product))
					the_encode.format(single ? cs_call_append_field : cs_call_append_field_switch)
						.arg(product.item->var)
						.arg(Model::Product::fake & field.item->product->clazz ? field.item->var : field.var)
						.write();
				else
					the_encode.format(single ? cs_call_append_static : cs_call_append_static_switch)
						.arg(field.item->var)
						.write();
			}

			if(!single)
				the_encode.write(cs_call_append_break_switch);
		}

		the_encode.write(single ? cs_decl_append_end : cs_decl_append_end_switch);
	}
	void write_functions_write()
	{
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			switch(product.clazz)
			{
			case Model::Product::list:
			{
				Model::Item& subitem = product.reduction_seq.at(1).element_seq.at(0).item;
				the_encode.format(cs_decl_write_list)
					.arg(product.item->type)
					.arg(product.item->var)
					.arg(subitem.type)
					.arg(subitem.var)
					.write();
				break;
			}
			case Model::Product::complex:
				write_function_write(product);
				break;
			case Model::Product::abstract:
				the_encode.format(cs_decl_write_abstract)
					.arg(product.item->type)
					.arg(product.item->var)
					.write();
				break;
			case Model::Product::string:
			case Model::Product::fake:
			case Model::Product::byte:
			case Model::Product::optional_list:
			case Model::Product::optional_complex:
			case Model::Product::optional_string:
			case Model::Product::optional_fake:
			case Model::Product::optional_byte:
			case Model::Product::optional_abstract:
				break;
			default:
				core::assert(false);
			}
		}
	}
	void write_function_write(Model::Product& product)
	{
		bool single = 1 == product.reduction_seq.size();
		the_encode.format(single ? cs_decl_write_begin : cs_decl_write_begin_switch)
			.arg(product.item->type)
			.arg(product.item->var)
			.write();

		core::Seq<Model::Reduction>::Iterator reduction_it = product.reduction_seq.head();
		while(reduction_it.next())
		{
			Model::Reduction& reduction = reduction_it.value();

			if(!single)
				the_encode.format(cs_call_write_case_switch).arg(reduction_it.position()).write();

			core::Seq<Model::Element>::Iterator field_it = reduction.element_seq.head();
			while(field_it.next())
			{
				Model::Element& field = field_it.value();
				if(core::is(field.item->product))
					the_encode.format(single ? cs_call_write_field : cs_call_write_field_switch)
						.arg(product.item->var)
						.arg(Model::Product::fake & field.item->product->clazz ? field.item->var : field.var)
						.write();
				else
					the_encode.format(single ? cs_call_write_static : cs_call_write_static_switch)
						.arg(field.item->var)
						.write();
			}

			if(!single)
				the_encode.write(cs_call_write_break_switch);
		}

		the_encode.write(single ? cs_decl_write_end : cs_decl_write_end_switch);
	}
	void write_functions_acquire()
	{
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			switch(product.clazz)
			{
			case Model::Product::list:
				the_encode.format(cs_acquire_list_arg0)
					.arg(product.item->type)
					.write();
			case Model::Product::complex:
			case Model::Product::string:
			case Model::Product::fake:
			case Model::Product::abstract:
				write_function_acquire(product);
				break;
			case Model::Product::byte:
			case Model::Product::optional_list:
			case Model::Product::optional_complex:
			case Model::Product::optional_string:
			case Model::Product::optional_fake:
			case Model::Product::optional_byte:
			case Model::Product::optional_abstract:
				break;
			default:
				core::assert(false);
			}
		}
	}
	void write_function_acquire(Model::Product& product)
	{
		core::Seq<Model::Reduction>::Iterator reduction_it = product.reduction_seq.head();
		while(reduction_it.next())
		{
			Model::Reduction& reduction = reduction_it.value();

			core::uint32 position = reduction_it.position();
			core::String acquire = Model::Product::abstract == product.clazz
				? cs_decl_virtual_acquire
				: cs_decl_acquire;
			the_encode.format(acquire)
				.arg(product.item->type)
				.arg(position ? core::String(position) : core::String::nil())
				.write();

			// args
			bool comma = false;
			core::Seq<Model::Element>::Iterator arg_it = reduction.element_seq.head();
			while(arg_it.next())
			{
				Model::Element& arg = arg_it.value();
				if(core::is(arg.item->product)
					&& (!(Model::Product::fake & arg.item->product->clazz) || (arg.var == arg.item->var)))
				{
					core::String decl = comma++ ? cs_decl_comma_arg : cs_decl_arg;
					the_encode.format(decl).arg(arg.item->type).arg(arg.var).write();
				}
			}

			// function body
			switch(product.clazz)
			{
			case Model::Product::list:
				if(product.item == reduction.element_seq.at(0).item)
					the_encode.format(cs_acquire_list_arg2)
						.arg(product.item->type)
						.arg(product.item->var)
						.arg(reduction.element_seq.at(reduction.element_seq.size() - 1).item->var)
						.write();
				else
					the_encode.format(cs_acquire_list_arg1)
						.arg(product.item->type)
						.arg(product.item->var)
						.arg(reduction.element_seq.at(0).item->var)
						.write();
				break;
			case Model::Product::complex:
				if(1 == product.reduction_seq.size())
					the_encode.format(cs_acquire_complex).arg(product.item->type).arg(position).write();
				else
					the_encode.format(cs_acquire_complex_reduction).arg(product.item->type).arg(position).write();
				break;
			case Model::Product::string:
			case Model::Product::fake:
				the_encode.format(cs_acquire_string).write();
				break;
			case Model::Product::abstract:
				the_encode.format(cs_acquire_abstract).arg(product.item->type).write();
				break;
			}

			// assign
			core::Seq<Model::Element>::Iterator field_it = reduction.element_seq.head();
			while(field_it.next())
			{
				Model::Element& field = field_it.value();
				switch(product.clazz)
				{
				case Model::Product::complex:
					if(core::is(field.item->product)
						&& (!(Model::Product::fake & field.item->product->clazz) || (field.item->var == field.var)))
						the_encode.format(cs_assign).arg(field.var).write();
					break;
				case Model::Product::string:
				case Model::Product::fake:
					bool unique = core::is(field.item->product)
						&& !(Model::Product::fake & field.item->product->clazz);
					the_encode.format(cs_append).arg(unique ? field.var : field.item->var).write();
					break;
				}
			}

			the_encode.write(cs_return_var);
		}
	}
	void write_put()
	{
		core::String input = (Model::verbose == the_model.level)
			? cs_input_machine_2d
			: cs_input_machine_1d;
		the_encode.format(cs_put_begin).arg(input).write();

		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();

			core::Seq<Model::Reduction>::Iterator reduction_it = product.reduction_seq.head();
			while(reduction_it.next())
			{
				Model::Reduction& reduction = reduction_it.value();
				core::uint32 position = reduction_it.position();

				// reduction begin
				the_encode.format(cs_reduction_case)
					.arg(product.item->type)
					.arg(reduction.id)
					.write();

				if(Model::Product::optional_byte & product.clazz)
				{
					if(!reduction.element_seq.size())
						the_encode.format(cs_empty_reduction).arg(product.item->var).write();
				}
				else
				{
					// acquire begin
					core::int32 heap_windup = -1;
					core::Seq<Model::Element>::Iterator element_it = reduction.element_seq.head();
					while(element_it.next())
					{
						Model::Element& element = element_it.value();
						heap_windup += core::nil(element.item->product) ? element.item->key.size() : 1;
					}
					core::String acquire;
					if(-1 == heap_windup)
						acquire = cs_call_acquire_increase_begin;
					else if(heap_windup)
						acquire = cs_call_acquire_decrease_begin;
					else
						acquire = cs_call_acquire_begin;
					the_encode.format(acquire)
						.arg(product.item->var)
						.arg(product.item->type)
						.arg(position ? core::String(position) : core::String::nil())
						.arg(heap_windup)
						.write();

					// acquire args
					bool comma = false;
					core::uint32 heap_offset = 0;
					core::Seq<Model::Element>::Iterator arg_it = reduction.element_seq.head();
					while(arg_it.next())
					{
						Model::Element& arg = arg_it.value();
						if(core::is(arg.item->product))
						{
							if(comma++)
								the_encode.write(cs_comma_space);
							if(heap_offset)
								the_encode.format(cs_cast_heap).arg(heap_offset).arg(arg.item->var).write();
							else
								the_encode.format(cs_cast_heap_direct).arg(arg.item->var).write();
							heap_offset++;
						}
						else
							heap_offset += arg.item->key.size();
					}

					// acquire end
					the_encode.write(cs_call_acquire_end);
				}

				// reduction end
				if(-1 == product.id)
					the_encode.format(cs_put_return).arg(product.item->var).write();
				else if(Model::verbose == the_model.level)
					the_encode.format(cs_state_machine_2d).arg(product.item->type).write();
				else
					the_encode.format(cs_state_machine_1d).arg(product.item->type).write();
			}
		}

		the_encode.write(Model::verbose == the_model.level ? cs_end_machine_2d : cs_check_machine_1d);
		the_encode.format(cs_put_end).arg(the_model.product->item->type).write();
	}
	void write_enum_product()
	{
		bool comma = false;
		the_encode.write(cs_enum_product_begin);

		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			if(comma++)
				the_encode.write(cs_comma);
			the_encode.format(cs_enum_product_item).arg(product.item->type).arg(product.id).write();
		}

		the_encode.write(cs_enum_product_end);
	}
	void write_enum_reduction()
	{
		bool comma = false;
		the_encode.write(cs_enum_reduction_begin);

		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			core::Seq<Model::Reduction>::Iterator event_it = product.reduction_seq.head();
			while(event_it.next())
			{
				if(comma++)
					the_encode.write(cs_comma);
				Model::Reduction& reduction = event_it.value();
				the_encode.format(cs_enum_reduction_item)
					.arg(product.item->type)
					.arg(reduction.id)
					.write();
			}
		}

		the_encode.write(cs_enum_reduction_end);
	}
	void write_union_heap()
	{
		the_encode.write(cs_struct_heap_begin);

		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			the_encode.format(cs_struct_heap_item).arg(product.item->type).arg(product.item->var).write();
		}

		the_encode.write(cs_struct_heap_end);
	}
private:
	Model& the_model;
	UniqueEncode the_encode;
	core::Region the_indent;
	core::HashSet<Machine::Node::share> the_node_set;
};

#endif // LANGEN_ENCODE_HPP
