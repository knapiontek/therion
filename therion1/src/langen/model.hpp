
#ifndef LANGEN_MODEL_HPP
#define LANGEN_MODEL_HPP

#include "core/core/string.hpp"
#include "core/core/seq.hpp"
#include "core/core/list.hpp"
#include "core/core/hashmap.hpp"
#include "core/core/treemap.hpp"

const core::uint32 the_average_reduction_size = 5;

struct Model
{
	struct Product;
	struct Input
	{
		bool unique;
		core::uint32 id;
		core::String name;
	};
	struct Item
	{
		typedef core::Shared<Item> share;
		bool unique;
		core::String key;
		core::String var;
		core::String type;
		core::Shared<Product> product;
	};
	struct Element
	{
		typedef core::Shared<Element> share;
		core::String var;
		Item::share item;
	};
	struct Reduction
	{
		typedef core::Shared<Reduction> share;
		core::uint16 line;
		core::uint32 id;
		core::Seq<Element> element_seq;
	};
	struct Product
	{
		typedef core::Shared<Product> share;
		enum
		{
			nil = 0x0,
			optional = 0x1,
			list = 0x2,
			complex = 0x4,
			string = 0x8,
			fake = 0x10,
			byte = 0x20,
			abstract = 0x40,
			optional_list = optional | list,
			optional_complex = optional | complex,
			optional_string = optional | string,
			optional_fake = optional | fake,
			optional_byte = optional | byte,
			optional_abstract = optional | abstract
		};
		core::uint32 clazz;
		core::int32 id;
		Item::share item;
		core::Seq<Reduction> reduction_seq;
	};
	enum Level
	{
		none,
		machine,
		verbose
	};
	Model() : terminal_list(0xF), product_list(0x2F), input_map(0x4F), item_map(0x4F)
	{

	}
	Product::share product;
	core::List<Item::share> terminal_list;
	core::List<Product> product_list;
	core::TreeMap<core::uint8, Input> input_map;
	core::HashMap<core::String, Item> item_map;
	Level level;
};

#endif // LANGEN_MODEL_HPP
