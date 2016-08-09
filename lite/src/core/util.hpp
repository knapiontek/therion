
// basic types

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;
typedef long double float128;

struct Nil
{
    Nil()
    {

    }
};

Nil nil;

// range of basic types

const int8 int8_min = -std::numeric_limits<int8>::min();
const int8 int8_max = std::numeric_limits<int8>::max();
const int8 int8_nil = std::numeric_limits<int8>::max();
const int16 int16_min = -std::numeric_limits<int16>::min();
const int16 int16_max = std::numeric_limits<int16>::max();
const int16 int16_nil = std::numeric_limits<int16>::max();
const int32 int32_min = -std::numeric_limits<int32>::min();
const int32 int32_max = std::numeric_limits<int32>::max();
const int32 int32_nil = std::numeric_limits<int32>::max();
const int64 int64_min = -std::numeric_limits<int64>::min();
const int64 int64_max = std::numeric_limits<int64>::max();
const int64 int64_nil = std::numeric_limits<int64>::max();
const uint8 uint8_min = -std::numeric_limits<uint8>::min();
const uint8 uint8_max = std::numeric_limits<uint8>::max();
const uint8 uint8_nil = std::numeric_limits<uint8>::max();
const uint16 uint16_min = -std::numeric_limits<uint16>::min();
const uint16 uint16_max = std::numeric_limits<uint16>::max();
const uint16 uint16_nil = std::numeric_limits<uint16>::max();
const uint32 uint32_min = -std::numeric_limits<uint32>::min();
const uint32 uint32_max = std::numeric_limits<uint32>::max();
const uint32 uint32_nil = std::numeric_limits<uint32>::max();
const uint64 uint64_min = -std::numeric_limits<uint64>::min();
const uint64 uint64_max = std::numeric_limits<uint64>::max();
const uint64 uint64_nil = std::numeric_limits<uint64>::max();
const float32 float32_nil = std::numeric_limits<float32>::signaling_NaN();
const float32 float32_inf = std::numeric_limits<float32>::infinity();
const float64 float64_nil = std::numeric_limits<float64>::signaling_NaN();
const float64 float64_inf = std::numeric_limits<float64>::infinity();
const float128 float128_nil = std::numeric_limits<float128>::signaling_NaN();
const float128 float128_inf = std::numeric_limits<float128>::infinity();

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
    const auto size = sizeof(Type);
    uint8 type[size];
    ::memcpy((void*)&type, &arg1, size);
    ::memcpy((void*)&arg1, &arg2, size);
    ::memcpy((void*)&arg2, &type, size);
}

inline int64 randomize(uint32 seed = 0)
{
    static bool init = true;
    if(init)
    {
        init = false;
        struct timeval time;
        ::gettimeofday(&time, 0);
        ::srand(time.tv_sec + seed);
    }
    return ::rand();
}

// memory management

template<class Type>
inline Type* acquire(int64 size)
{
    Type* type = (Type*)::malloc(size);
    if(!type)
        Handler::handler()->throw_alloc_exception();
    return type;
}

template<class Type>
inline Type* acquire(Type* type, int64 size)
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
