
#ifndef CORE_SHARE_HPP
#define CORE_SHARE_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

template<typename Type>
class Shared
{
	template<typename Other>
	friend class Shared;
public:
	Shared()
	{
		the_type = 0;
	}
	Shared(const Nil&)
	{
		the_type = 0;
	}
	Shared(const Type& type)
	{
		the_type = const_cast<Type*>(&type);
	}
	Shared(const Shared& arg)
	{
		the_type = arg.the_type;
	}
	template<class Other>
	Shared(const Shared<Other>& arg)
	{
		the_type = static_cast<Type*>(arg.the_type);
	}
	Shared& operator=(const Nil& nil)
	{
		the_type = 0;
		return *this;
	}
	Shared& operator=(const Type& type)
	{
		the_type = const_cast<Type*>(&type);
		return *this;
	}
	Shared& operator=(const Shared& arg)
	{
		the_type = arg.the_type;
		return *this;
	}
	template<class Other>
	Shared& operator=(const Shared<Other>& arg)
	{
		the_type = static_cast<Type*>(arg.the_type);
		return *this;
	}
	friend bool operator==(const Nil&, const Shared& arg)
	{
		return (0 == arg.the_type);
	}
	friend bool operator!=(const Nil&, const Shared& arg)
	{
		return (0 != arg.the_type);
	}
	bool operator==(const Nil&) const
	{
		return (0 == the_type);
	}
	bool operator!=(const Nil&) const
	{
		return (0 != the_type);
	}
	bool operator==(const Shared& arg) const
	{
		return (the_type == arg.the_type);
	}
	bool operator!=(const Shared& arg) const
	{
		return (the_type != arg.the_type);
	}
	template<class Other>
	bool operator==(const Shared<Other>& arg) const
	{
		return (the_type == static_cast<Type*>(arg.the_type));
	}
	template<class Other>
	bool operator!=(const Shared<Other>& arg) const
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

