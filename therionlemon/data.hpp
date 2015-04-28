
#ifndef DAISY_DATA_HPP
#define DAISY_DATA_HPP

#include "core/out.hpp"
#include "core/map.hpp"
#include "core/list.hpp"
#include "core/pager.hpp"

typedef core::String Token;

class Type
{
public:
	enum Enum
	{ BOOL, INT8, INT16, INT32, INT64, INT128, UINT8, UINT16, UINT32, UINT64, UINT128, FLOAT32, FLOAT64, FLOAT128, CLASS };
public:
	Enum type;
	class Class* clazz;
	int size;
};

class Arg
{
public:
	Type* type;
	Token* name;
};

typedef core::Map<Token*, Arg*> ArgMap;

class Func
{
public:
	Type* out;
	Token* name;
	ArgMap* arg_map;
};

typedef core::Map<Token*, Func*> FuncMap;

class Decl
{
public:
	enum Enum
	{ NONE, FAST, UNIQUE };
public:
	Enum modifier;
	Type* type;
	Token* name;
};

typedef core::Map<Token*, Decl*> DeclMap;

class Class
{
public:
	Token* name;
	DeclMap* decl_map;
};

typedef core::Map<Token*, Class*> ClassMap;

class ErrorHandler
{
public:
	virtual ~ErrorHandler()
	{

	}
	virtual core::String message()
	{
		return 0;
	}
};

class Data
{
public:
	Data(ErrorHandler* error_handler)
	{
		the_error_handler = error_handler;
	}
	~Data()
	{
		the_pager.release();
	}
	const ClassMap& class_map() const
	{
		return the_class_map;
	}
	void error() const
	{
		core::out::print_line(the_error_handler->message());
	}
public:
	// Type
	Type* create_type(Type::Enum _type)
	{
		Type* type = the_pager.acquire<Type>();
		type->type = _type;
		type->clazz = 0;
		type->size = 0;
		return type;
	}
	Type* create_type(Token* class_name)
	{
		Type* type = the_pager.acquire<Type>();
		type->type = Type::CLASS;
		type->size = 0;
		type->clazz = the_class_map.lookup(class_name, 0);
		if(!type->clazz)
		{
			sys::Con("class '%1' not found\n%2")
				.arg(class_name->content()) not eficient
				.arg(class_name) let use pointers instead of types
				.arg(the_error_handler->message())
				.endln();
		}
		return type;
	}
	Type* type_size(Type* type)
	{
		type->size = 0;
		return type;
	}
	Type* type_size(Type* type, Token* size)
	{
		type->size = size->to_int();
		return type;
	}
	// Arg
	Arg* create_arg(Type* type, Token* name)
	{
		Arg* arg = the_pager.acquire<Arg>();
		arg->type = type;
		arg->name = name;
		return arg;
	}
	ArgMap* create_arg_map(Arg* arg)
	{
		ArgMap* arg_map = &the_arg_store.acquire();
		arg_map->at(arg->name) = arg;
		return arg_map;
	}
	ArgMap* insert_arg(ArgMap* arg_map, Arg* arg)
	{
		arg_map->at(arg->name) = arg;
		return arg_map;
	}
	// Func
	Func* create_func(Type* out, Token* name, ArgMap* arg_map)
	{
		Func* func = the_pager.acquire<Func>();
		func->out = out;
		func->name = name;
		func->arg_map = arg_map;
		return func;
	}
	void insert_func(Func* func)
	{
		the_func_map.at(func->name) = func;
	}
	// Decl
	Decl* create_decl(Decl::Enum modifier, Type* type, Token* name)
	{
		Decl* decl = the_pager.acquire<Decl>();
		decl->modifier = modifier;
		decl->type = type;
		decl->name = name;
		return decl;
	}
	DeclMap* create_decl_map(Decl* decl)
	{
		DeclMap* decl_map = &the_decl_store.acquire();
		decl_map->at(decl->name) = decl;
		return decl_map;
	}
	DeclMap* insert_decl(DeclMap* decl_map, Decl* decl)
	{
		decl_map->at(decl->name) = decl;
		return decl_map;
	}
	// Class
	Class* create_class(Token* name, DeclMap* decl_map)
	{
		Class* clazz = the_pager.acquire<Class>();
		clazz->name = name;
		clazz->decl_map = decl_map;
		return clazz;
	}
	void insert_class(Class* clazz)
	{
		the_class_map.at(clazz->name) = clazz;
	}
protected:
	ErrorHandler* the_error_handler;
	core::Pager the_pager;
	core::List<DeclMap> the_decl_store;
	ClassMap the_class_map;
	core::List<ArgMap> the_arg_store;
	FuncMap the_func_map;
};

#endif // DAISY_DATA_HPP
