
#ifndef LANGEN_PROTOTYPE_HPP
#define LANGEN_PROTOTYPE_HPP

#include "langen/mangle.hpp"
#include "langen/model.hpp"
#include "langen/cs.hpp"
#include "core/io/file.hpp"
#include "core/core/hashmap.hpp"

class ClazzSeq
{
public:
	static core::uint32 lookup(core::String& name)
	{
		static ClazzSeq type_seq;
		Pair::share pair = type_seq.the_seq.lookup((Pair&)name);
		return core::is(pair)
			? pair->clazz
			: Model::Product::nil;
	}
private:
	struct Pair
	{
		typedef core::Shared<Pair> share;
		core::MutableString name;
		core::uint32 clazz;
	};
private:
	ClazzSeq()
	{
		static const Pair types[] = {
			{ "class-list", Model::Product::list },
			{ "byte-list", Model::Product::string },
			{ "class", Model::Product::complex },
			{ "byte", Model::Product::byte },
			{ "alias", Model::Product::fake },
			{ "", Model::Product::abstract }
		};
		core::uint32 size = sizeof(types) / sizeof(Pair);
		the_seq.size(size);
		for(core::uint32 i = 0; i < size; i++)
			the_seq.at(i) = types[i];
		the_seq.sort();
	}
private:
	typedef core::CastIndex<core::String> StringIndex;
	core::Seq<Pair, StringIndex> the_seq;
};

class Prototype
{
public:
	static void read(Model& model, core::String& filename)
	{
		Prototype prototype(model, filename);
		prototype.read();
		prototype.analyse_tails();
		prototype.analyse_items();
		prototype.analyse_reductions();
		prototype.count_inputs();
	}
private:
	Prototype(Model& model, core::String& filename)
		: the_model(model),	the_filename(filename)
	{

	}
	void read()
	{
		// read entire input file
		io::File file(the_filename);
		file.open(io::File::read_only);
		core::uint32 file_size = file.size();
		core::String file_content;
		file_content.size(file_size);
		io::Decode decode = file.input();
		decode.read(file_content);
		file.close();

		// simplify analysis
		file_content.append(cs_line_zero);

		// analyse the file content
		core::uint32 line_cnt = 0;
		Model::Item::share item;
		Model::Reduction::share reduction;
		core::String::Iterator it = file_content.head();
		while(true)
		{
			// line
			line_cnt++;
			while(it.next() && '\n' == it.value())
				line_cnt++;
			if('\0' == it.value())
				return;

			// product
			if(::isalpha(it.value()))
			{
				core::uint32 pos = it.position();
				while(it.next() && ::isalnum(it.value()));
				core::String key(file_content.data() + pos, it.position() - pos);
				item = the_model.item_map.at(key);
				item->unique = core::nil(item->key);
				if(item->unique)
					item->key = key;
				if(core::nil(item->product))
				{
					item->product = the_model.product_list.append();
					item->product->clazz = Model::Product::nil;
					item->product->item = item;
				}
				if(core::nil(the_model.product))
					the_model.product = item->product;

				// product clazz
				pos = 1 + it.position();
				while(it.next() && '}' != it.value());
				core::String clazz_name(file_content.data() + pos, it.position() - pos);
				core::uint32 clazz = ClazzSeq::lookup(clazz_name);
				if(Model::Product::nil == clazz
					|| (item->product->clazz != clazz && Model::Product::nil != item->product->clazz))
					env::Fail(cs_syntax_error).arg(the_filename).arg(line_cnt).fire();
				item->product->clazz = clazz;

				if(it.next() && '\n' == it.value())
					continue;
				else if(' ' != it.value() || !it.next())
					env::Fail(cs_syntax_error).arg(the_filename).arg(line_cnt).fire();
			}

			// reduction
			if(core::nil(item))
				env::Fail(cs_syntax_error).arg(the_filename).arg(line_cnt).fire();
			if(':' == it.value())
			{
				reduction = item->product->reduction_seq.append();
				reduction->line = line_cnt;
				if(it.next() && '\n' == it.value())
					continue;
				else if(' ' != it.value())
					env::Fail(cs_syntax_error).arg(the_filename).arg(line_cnt).fire();
			}
			else if(core::nil(reduction) || ' ' != it.value() || !it.next() || ' ' != it.value())
				env::Fail(cs_syntax_error).arg(the_filename).arg(line_cnt).fire();

			// elements
			while('\n' != it.value())
			{
				if(!it.next() || ' ' == it.value())
					env::Fail(cs_syntax_error).arg(the_filename).arg(line_cnt).fire();
				core::uint32 pos = it.position();
				while(it.next() && ('\n' != it.value() && ' ' != it.value()));
				core::String key(file_content.data() + pos, it.position() - pos);
				if(!Mangle::unhex(key))
					env::Fail(cs_while_unhexing).arg(the_filename).arg(line_cnt).arg(key).fire();
				Model::Element& element = reduction->element_seq.append();
				element.item = the_model.item_map.at(key);
				element.item->unique = core::nil(element.item->key);
				if(element.item->unique)
					element.item->key = key;
			}
		}
	}
	void analyse_tails()
	{
		core::HashMap<core::String, Model::Item>::Iterator item_it = the_model.item_map.head();
		while(item_it.next())
		{
			Model::Item& item = item_it.value();
			core::uint32 key_last_pos = item.key.size() - 1;
			core::uint8 key_last_byte = item.key.at(key_last_pos);
			if('*' == key_last_byte)
			{
				core::String base_key(item.key.data(), key_last_pos);
				Model::Item::share base_item = the_model.item_map.lookup(base_key);
				if(core::is(base_item) && core::is(base_item->product))
				{
					core::String plus_key = core::Format().arg(base_key).arg(cs_ps).end();
					Model::Item& plus_item = the_model.item_map.at(plus_key);
					if(core::nil(plus_item.key))
						plus_item.key = plus_key;

					item.key = core::Format().arg(base_key).arg(cs_star).end();

					Model::Product& product = the_model.product_list.append();
					product.clazz = Model::Product::optional;
					product.clazz |= (Model::Product::byte == base_item->product->clazz)
						? Model::Product::string
						: Model::Product::complex;
					product.item = item;

					Model::Reduction& reduction = product.reduction_seq.append();
					Model::Element& element = reduction.element_seq.append();
					element.item = plus_item;

					product.reduction_seq.append();

					item.product = product;
				}
			}
			else if('+' == key_last_byte)
			{
				core::String base_key(item.key.data(), key_last_pos);
				Model::Item::share base_item = the_model.item_map.lookup(base_key);
				if(core::is(base_item) && core::is(base_item->product))
				{
					item.key = core::Format().arg(base_key).arg(cs_plus).end();

					Model::Product& product = the_model.product_list.append();
					product.clazz = (Model::Product::byte == base_item->product->clazz)
						? Model::Product::string
						: Model::Product::list;
					product.item = item;

					Model::Reduction& reduction0 = product.reduction_seq.append();
					Model::Element& element00 = reduction0.element_seq.append();
					Model::Element& element01 = reduction0.element_seq.append();
					element00.item = item;
					element01.item = base_item;

					Model::Reduction& reduction1 = product.reduction_seq.append();
					Model::Element& element10 = reduction1.element_seq.append();
					element10.item = base_item;

					item.product = product;
				}
			}
			else if('?' == key_last_byte)
			{
				core::String base_key(item.key.data(), key_last_pos);
				Model::Item::share base_item = the_model.item_map.lookup(base_key);
				if(core::is(base_item) && core::is(base_item->product))
				{
					item.key = core::Format().arg(base_key).arg(cs_ques).end();

					Model::Product& product = the_model.product_list.append();
					product.clazz = Model::Product::optional | base_item->product->clazz;
					product.item = item;

					Model::Reduction& reduction = product.reduction_seq.append();
					Model::Element& element = reduction.element_seq.append();
					element.item = base_item;

					product.reduction_seq.append();

					item.product = product;
				}
			}
		}
	}
	void analyse_items()
	{
		core::HashMap<core::String, Model::Item>::Iterator item_it = the_model.item_map.head();
		while(item_it.next())
		{
			Model::Item& item = item_it.value();
			if(core::is(item.product))
			{
				item.type = item.key;
				item.var = Mangle::type2var(item.key);
			}
			else
			{
				core::String::Iterator byte_it = item.key.head();
				while(byte_it.next())
				{
					core::uint8 byte = byte_it.value();
					Model::Input& input = the_model.input_map.at(byte);
					input.unique = core::nil(input.name);
					if(input.unique)
					{
						input.id = 0;
						input.name = Mangle::hex(byte);
					}
					input.unique &= item.unique;
				}
				bool byte = 1 == item.key.size() && 0 != item.key.at(0);
				item.type = byte ? cs_byte : cs_string;
				item.var = Mangle::value2var(item.key);
				the_model.terminal_list.append(item);
			}
		}
	}
	void analyse_reductions()
	{
		core::uint32 reduction_cnt = 1;
		core::List<Model::Product>::Iterator product_it = the_model.product_list.head();
		while(product_it.next())
		{
			Model::Product& product = product_it.value();
			product.id = product_it.position() - 1; // first never used

			if(Model::none != the_model.level && Model::Product::fake == product.clazz)
				env::Fail(cs_fake_product).arg(product.item->key).fire();

			core::List<core::uint32> erase_list(the_average_reduction_size); // erase compressed reductions
			core::Seq<Model::Reduction>::Iterator reduction_it = product.reduction_seq.head();
			while(reduction_it.next())
			{
				Model::Reduction& reduction = reduction_it.value();
				if(reduction.element_seq.size() && Model::Product::byte & product.clazz)
				{
					if(1 != reduction.element_seq.size())
						env::Fail(cs_only_bytes_expected)
							.arg(the_filename)
							.arg(reduction.line)
							.arg(product.item->key)
							.fire();

					Model::Element& element = reduction.element_seq.at(0);
					if(core::nil(element.item->product))
					{
						if(1 != element.item->key.size())
							env::Fail(cs_byte_expected)
								.arg(the_filename)
								.arg(reduction.line)
								.arg(product.item->key)
								.arg(element.item->key)
								.fire();

						core::uint8 byte = element.item->key.at(0);
						Model::Input& input = the_model.input_map.at(byte);
						if(input.unique) // compression
						{
							input.id = product.id;
							erase_list.append(reduction_it.position());
						}
						else
						{
							reduction.id = reduction_cnt++;
							reduction.element_seq.squeeze();
						}
					}
					else if(Model::Product::byte & element.item->product->clazz)
					{
						reduction.element_seq.squeeze();
						reduction.id = reduction_cnt++;
					}
					else
						env::Fail(cs_byte_expected)
							.arg(the_filename)
							.arg(reduction.line)
							.arg(product.item->key)
							.arg(element.item->key)
							.fire();
				}
				else // append extra number to identify element in struct or function
				{
					reduction.element_seq.squeeze();
					reduction.id = reduction_cnt++;

					core::HashMap<core::String, core::uint32> var_map(the_average_reduction_size);
					core::Seq<Model::Element>::Iterator element_it = reduction.element_seq.head();
					while(element_it.next())
					{
						Model::Element& element = element_it.value();
						element.var = element.item->var;
						core::uint32 cnt = ++var_map.at(element.var, core::uint32_max);
						if(cnt)
							element.var.append(cnt);
					}
				}
			}
			product.reduction_seq.erase_by_iterator(erase_list.head());
		}
	}
	void count_inputs()
	{
		core::assert(-1 == the_model.product->id); // next line assumes it
		core::uint32 input_id_cnt = the_model.product_list.size() - 1;
		core::TreeMap<core::uint8, Model::Input>::Iterator input_it = the_model.input_map.head();
		while(input_it.next())
		{
			Model::Input& input = input_it.value();
			if(!input.id)
				input.id = input_id_cnt++;
		}
	}
private:
	Model& the_model;
	core::String the_filename;
};

#endif // LANGEN_PROTOTYPE_HPP
