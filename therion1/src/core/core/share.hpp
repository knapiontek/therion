
#ifndef CORE_SHARE_HPP
#define CORE_SHARE_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

template<typename Type>
class Shared
{
	template<typename Extension>
	friend class Shared;
public:
	static const Shared& nil()
	{
		static Shared nil;
		return nil;
	}
public:
	Shared()
	{
		the_type = 0;
	}
	Shared(const Type& type)
	{
		the_type = (Type*)&type;
	}
	Shared(const Shared& arg)
	{
		the_type = arg.the_type;
	}
	template<class Extension>
	Shared(const Shared<Extension>& arg)
	{
		the_type = static_cast<Type*>(arg.the_type);
	}
	Shared& operator=(const Type& type)
	{
		the_type = (Type*)&type;
		return *this;
	}
	Shared& operator=(const Shared& arg)
	{
		the_type = arg.the_type;
		return *this;
	}
	template<class Extension>
	Shared& operator=(const Shared<Extension>& arg)
	{
		the_type = static_cast<Type*>(arg.the_type);
		return *this;
	}
	bool operator==(const Shared& arg) const
	{
		return (the_type == arg.the_type);
	}
	bool operator!=(const Shared& arg) const
	{
		return (the_type != arg.the_type);
	}
	template<class Extension>
	bool operator==(const Shared<Extension>& arg) const
	{
		return (the_type == static_cast<Type*>(arg.the_type));
	}
	template<class Extension>
	bool operator!=(const Shared<Extension>& arg) const
	{
		return (the_type != static_cast<Type*>(arg.the_type));
	}
	Type* operator->()
	{
		assert(the_type);
		return the_type;
	}
	operator Type&()
	{
		assert(the_type);
		return *the_type;
	}
private:
	Type* the_type;
};

template<typename Type>
inline bool nil(const Shared<Type>& arg)
{
	return arg == Shared<Type>::nil();
}

template<typename Type>
inline bool is(const Shared<Type>& arg)
{
	return arg != Shared<Type>::nil();
}

namespace share
{
	typedef Shared<int8> int8;
	typedef Shared<int16> int16;
	typedef Shared<int32> int32;
	typedef Shared<int64> int64;

	typedef Shared<uint8> uint8;
	typedef Shared<uint16> uint16;
	typedef Shared<uint32> uint32;
	typedef Shared<uint64> uint64;

	typedef Shared<float32> float32;
	typedef Shared<float64> float64;
	typedef Shared<float128> float128;
}

NAMESPACE_END(core);

#endif // CORE_SHARE_HPP
