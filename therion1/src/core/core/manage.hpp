
#ifndef CORE_MANAGE_HPP
#define CORE_MANAGE_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

template<typename Type>
class Managed
{
	template<typename Extension>
	friend class Managed;
public:
	static const Managed& nil()
	{
		static uint32 cnt[1] = { 1 };
		static Handle* handle = (Handle*)&cnt;
		return (Managed&)handle;
	}
public:
	Managed()
	{
		the_handle = acquire<Handle>(sizeof(Handle));
		the_handle->cnt = 1;
		new((void*)the_handle->type) Type();
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
	template<class Extension>
	Managed(const Managed<Extension>& arg)
	{
		assert(static_cast<Type*>(arg.the_handle->type));
		the_handle = arg.the_handle;
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
	Managed& operator=(const Type& type)
	{
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
	template<class Extension>
	Managed& operator=(const Managed<Extension>& arg)
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
	bool operator==(const Managed& arg) const
	{
		return (the_handle == arg.the_handle);
	}
	bool operator!=(const Managed& arg) const
	{
		return (the_handle != arg.the_handle);
	}
	template<class Extension>
	bool operator==(const Managed<Extension>& arg) const
	{
		assert(static_cast<Type*>(arg.the_handle->type));
		return (the_handle == (Handle*)arg.the_handle);
	}
	template<class Extension>
	bool operator!=(const Managed<Extension>& arg) const
	{
		assert(static_cast<Type*>(arg.the_handle->type));
		return (the_handle != (Handle*)arg.the_handle);
	}
	Type* operator->()
	{
		assert(the_handle != nil().the_handle);
		return the_handle->type;
	}
	operator Type&()
	{
		assert(the_handle != nil().the_handle);
		return *the_handle->type;
	}
private:
	struct Handle
	{
		uint32 cnt;
		Type type[1];
	};
private:
	Handle* the_handle;
};

template<typename Type>
inline bool nil(const Managed<Type>& arg)
{
	return arg == Managed<Type>::nil();
}

template<typename Type>
inline bool is(const Managed<Type>& arg)
{
	return arg != Managed<Type>::nil();
}

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
