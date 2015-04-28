
#ifndef CORE_MANAGE_HPP
#define CORE_MANAGE_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

template<typename Type>
class Managed
{
	template<typename Other>
	friend class Managed;
public:
	Managed()
	{
		the_handle = acquire<Handle>(sizeof(Handle));
		the_handle->cnt = 1;
		new((void*)the_handle->type) Type();
	}
	Managed(const Nil&)
	{
		the_handle = nil_handle();
		the_handle->cnt++;
	}
	Managed(const Type& type)
	{
		the_handle = acquire<Handle>(sizeof(Handle));
		the_handle->cnt = 1;
		new((void*)the_handle->type) Type(type);
	}
	Managed(const Managed& arg)
	{
		the_handle = arg.the_handle;
		the_handle->cnt++;
	}
	template<class Other>
	Managed(const Managed<Other>& arg)
	{
		assert(static_cast<Type*>(arg.the_handle->type));
		the_handle = (Handle*)arg.the_handle;
		the_handle->cnt++;
	}
	~Managed()
	{
		if(!--the_handle->cnt)
		{
			Type* type = the_handle->type;
			type->~Type(); // gcc bug
			release<Handle>(the_handle);
		}
	}
	Managed& operator=(const Nil&)
	{
		nil_handle()->cnt++;
		if(!--the_handle->cnt)
		{
			Type* type = the_handle->type;
			type->~Type(); // gcc bug
			release<Handle>(the_handle);
		}
		the_handle = nil_handle();
		return *this;
	}
	Managed& operator=(const Type& type)
	{
		assert(the_handle != nil_handle());
		*the_handle->type = type;
		return *this;
	}
	Managed& operator=(const Managed& arg)
	{
		arg.the_handle->cnt++;
		if(!--the_handle->cnt)
		{
			Type* type = the_handle->type;
			type->~Type(); // gcc bug
			release<Handle>(the_handle);
		}
		the_handle = arg.the_handle;
		return *this;
	}
	template<class Other>
	Managed& operator=(const Managed<Other>& arg)
	{
		assert(static_cast<Type*>(arg.the_handle->type));
		arg.the_handle->cnt++;
		if(!--the_handle->cnt)
		{
			Type* type = the_handle->type;
			type->~Type(); // gcc bug
			release<Handle>(the_handle);
		}
		the_handle = (Handle*)arg.the_handle;
		return *this;
	}
	friend bool operator==(const Nil&, const Managed& arg)
	{
		return (nil_handle() == arg.the_handle);
	}
	friend bool operator!=(const Nil&, const Managed& arg)
	{
		return (nil_handle() != arg.the_handle);
	}
	bool operator==(const Nil&) const
	{
		return (the_handle == nil_handle());
	}
	bool operator!=(const Nil&) const
	{
		return (the_handle != nil_handle());
	}
	bool operator==(const Managed& arg) const
	{
		return (the_handle == arg.the_handle);
	}
	bool operator!=(const Managed& arg) const
	{
		return (the_handle != arg.the_handle);
	}
	template<class Other>
	bool operator==(const Managed<Other>& arg) const
	{
		return (the_handle == (Handle*)arg.the_handle);
	}
	template<class Other>
	bool operator!=(const Managed<Other>& arg) const
	{
		return (the_handle != (Handle*)arg.the_handle);
	}
	Type* operator->()
	{
		assert(the_handle != nil_handle());
		return the_handle->type;
	}
	operator Type&()
	{
		assert(the_handle != nil_handle());
		return *the_handle->type;
	}
private:
	struct Handle
	{
		uint32 cnt;
		Type type[1];
	};
private:
	static Handle* nil_handle()
	{
		static uint32 nil[1] = { 1 };
		return (Handle*)&nil;
	}
private:
	Handle* the_handle;
};

namespace manage
{
	typedef Managed<int8> int8;
	typedef Managed<int16> int16;
	typedef Managed<int32> int32;
	typedef Managed<int64> int64;

	typedef Managed<uint8> uint8;
	typedef Managed<uint16> uint16;
	typedef Managed<uint32> uint32;
	typedef Managed<uint64> uint64;

	typedef Managed<float32> float32;
	typedef Managed<float64> float64;
	typedef Managed<float128> float128;
}

NAMESPACE_END(core);

#endif // CORE_MANAGE_HPP

