
#ifndef DAISY_BDB_HPP
#define DAISY_BDB_HPP

#include "daisy/data.hpp"
#include "io/file.hpp"

const char* type_name(Type::Enum type)
{
	static const char* names[] =
	{
		"bool", "int8", "int16", "int32", "int64", "int128",
		"uint8", "uint16", "uint32", "uint64", "uint128",
		"float32", "float64", "float128", "class" };
	return names[type];
}

class Bdb
{
public:
	Bdb(String& file_name)
	{
		the_file_name = file_name;
	}
	void run(Data& data)
	{
		String header_file_name;
		header_file_name.clone(the_file_name);
		header_file_name.append(".hpp");
		FileOutput out(header_file_name);
		ClassMap& class_map = data.class_map();
		const ClassMap::Iterator class_it = class_map.head();
		while(class_it.has_next())
		{
			const Class* clazz = class_it.next();
			out.write("\nclass ")
			out.write(*clazz->name)
			out.write("
			{\n");

			DeclMap* decl_map = clazz->decl_map;
			const DeclMap::Iterator decl_it = decl_map->head();
			while(decl_it.has_next())
			{
				const Decl* decl = decl_it.next();
				String arg = Format("%1%2\n\t%3%4\n")
					.arg(type_name(decl->type->type))
					.arg(decl->name)
					.arg(type_name(decl->type->type))
					.arg(decl->name)
					.end();
				out.write(arg);
			}
		}
	}
private:
	core::String the_file_name;
};

#endif // DAISY_BDB_HPP
