
#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <new>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END(name) }
#define NAMESPACE(name) using namespace name

NAMESPACE_BEGIN(core);

// basic types

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;
typedef long double float128;

struct Nil
{
	Nil()
	{

	}
};

const Nil nil;

// range of basic types

const int8 int8_max = 127;
const int8 int8_min = (-int8_max - 1);
const int8 int8_nil = int8_max;
const int16 int16_max = 32767;
const int16 int16_min = (-int16_max - 1);
const int16 int16_nil = int16_max;
const int32 int32_max = 2147483647L;
const int32 int32_min = (-int32_max - 1L);
const int32 int32_nil = int32_max;
const int64 int64_max = 9223372036854775807LL;
const int64 int64_min = (-int64_max - 1LL);
const int64 int64_nil = int64_max;
const uint8 uint8_min = 0;
const uint8 uint8_max = 255;
const uint8 uint8_nil = uint8_max;
const uint16 uint16_min = 0;
const uint16 uint16_max = 65535;
const uint16 uint16_nil = uint16_max;
const uint32 uint32_min = 0UL;
const uint32 uint32_max = 4294967295UL;
const uint32 uint32_nil = uint32_max;
const uint64 uint64_min = 0ULL;
const uint64 uint64_max = 18446744073709551615ULL;
const uint64 uint64_nil = uint64_max;
const float32 float32_nil = NAN;
const float32 float32_inf = INFINITY;
const float64 float64_nil = NAN;
const float64 float64_inf = INFINITY;
const float128 float128_nil = NAN;
const float128 float128_inf = INFINITY;

// cross-dependancy resolver

class Handler
{
public:
	static Handler*& handler()
	{
		static Handler* handler = 0;
		return handler;
	}
public:
	virtual void throw_alloc_exception() = 0;
	virtual void call_assert(const char* file_name, int line_no, const char* content) = 0;
};

// miscellaneous

template <typename Type>
inline Type& min(const Type& arg1, const Type& arg2)
{
	return (arg1 < arg2) ? (Type&)arg1 : (Type&)arg2;
}

template <typename Type>
inline Type& max(const Type& arg1, const Type& arg2)
{
	return (arg1 > arg2) ? (Type&)arg1 : (Type&)arg2;
}

template <typename Type>
inline void xchange(Type& arg1, Type& arg2)
{
	const uint32 size = sizeof(Type);
	uint8 type[size];
	::memcpy((void*)&type, &arg1, size);
	::memcpy((void*)&arg1, &arg2, size);
	::memcpy((void*)&arg2, &type, size);
}

inline uint32 randomize()
{
	static bool init = true;
	if(init)
	{
		init = false;
		struct timeval time;
		::gettimeofday(&time, 0);
		::srand(time.tv_sec);
	}
	return ::rand();
}

// memory management

template<class Type>
inline Type* acquire(uint32 size)
{
	Type* type = (Type*)::malloc(size);
	if(!type)
		Handler::handler()->throw_alloc_exception();
	return type;
}

template<class Type>
inline Type* acquire(Type* type, uint32 size)
{
	type = (Type*)::realloc((void*)type, size);
	if(!type)
		Handler::handler()->throw_alloc_exception();
	return type;
}

template<class Type>
inline void release(Type* type)
{
	::free((void*)type);
}

// assert/verify

inline void call_assert(bool expr, const char* file_name, int line_no, const char* content)
{
	if(!expr)
		Handler::handler()->call_assert(file_name, line_no, content);
}

inline void call_nil()
{

}

#ifndef ASSERT
	#define assert(expr) call_assert((expr), __FILE__, __LINE__, __STRING(expr))
#else
	#define assert(expr) call_nil()
#endif

#define verify(expr) call_assert((expr), __FILE__, __LINE__, __STRING(expr))

NAMESPACE_END(core);

#endif // CORE_CORE_HPP

