
#ifndef CORE_STRING_HPP
#define CORE_STRING_HPP

#include "core/core/convert.hpp"
#include "core/core/share.hpp"
#include "core/core/manage.hpp"
#include <ctype.h>

NAMESPACE_BEGIN(core);

class MutableString
{
	struct Handle;
public:
	typedef Shared<const MutableString> share;
	typedef Managed<const MutableString> manage;
public:
	class Iterator
	{
		friend class MutableString;
	public:
		bool head()
		{
			return the_pos < the_handle->data;
		}
		bool tail()
		{
			return the_pos == the_handle->data + the_handle->size;
		}
		bool next()
		{
			return ++the_pos < the_handle->data + the_handle->size;
		}
		bool prev()
		{
			return the_pos-- > the_handle->data;
		}
		uint32 position()
		{
			return the_pos - the_handle->data;
		}
		uint8& value()
		{
			return *the_pos;
		}
	private:
		Iterator(uint8* pos, Handle* handle)
		{
			the_pos = pos;
			the_handle = handle;
		}
	private:
		uint8* the_pos;
		Handle* the_handle;
	};
public:
	static const MutableString& nil()
	{
		static MutableString nil;
		return nil;
	}
public:
	MutableString()
	{
		static Handle handle = { 1, 0, { 0 } };
		the_handle = &handle;
		the_handle->cnt++;
	}
	MutableString(const MutableString& arg)
	{
		the_handle = arg.the_handle;
		the_handle->cnt++;
	}
	MutableString(Region& arg)
	{
		the_handle = clone(arg.data, arg.size);
	}
	MutableString(uint8* arg, uint32 size)
	{
		the_handle = clone(arg, size);
	}
	MutableString(const char* arg)
	{
		uint32 size = ::strlen(arg);
		the_handle = clone((uint8*)arg, size);
	}
	explicit MutableString(bool arg)
	{
		uint8 buffer[bool_max_digit];
		uint8* data = buffer;
		uint32 size = bool_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(int8 arg)
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(int16 arg)
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(int32 arg)
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(int64 arg)
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(uint8 arg)
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(uint16 arg)
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(uint32 arg)
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(uint64 arg)
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		the_handle = clone(data, size);
	}
	explicit MutableString(float32 arg, uint8 precision = float32_precision)
	{
		uint8 buffer[float32_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float32_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		the_handle = clone(data, size);
	}
	explicit MutableString(float64 arg, uint8 precision = float64_precision)
	{
		uint8 buffer[float64_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float64_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		the_handle = clone(data, size);
	}
	explicit MutableString(float128 arg, uint8 precision = float128_precision)
	{
		uint8 buffer[float128_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float128_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		the_handle = clone(data, size);
	}
	~MutableString()
	{
		if(!--the_handle->cnt)
			release<Handle>(the_handle);
	}
	Iterator head() const
	{
		return Iterator(the_handle->data - 1, the_handle);
	}
	Iterator tail() const
	{
		return Iterator(the_handle->data + the_handle->size, the_handle);
	}
	uint8* data() const
	{
		return the_handle->data;
	}
	char* ascii() const
	{
		return (char*)the_handle->data;
	}
	uint8& at(uint32 pos) const
	{
		assert(pos < the_handle->size);
		return the_handle->data[pos];
	}
	uint32 size() const
	{
		return the_handle->size;
	}
	void size(uint32 size) const
	{
		the_handle = --the_handle->cnt
			? acquire<Handle>(sizeof(Handle) + sizeof(uint8) * size)
			: acquire<Handle>(the_handle, sizeof(Handle) + sizeof(uint8) * size);
		the_handle->cnt = 1;
		the_handle->size = size;
		the_handle->data[size] = '\0';
	}
	bool is_empty() const
	{
		return !the_handle->size;
	}
	MutableString clone() const
	{
		return MutableString(the_handle->data, the_handle->size);
	}
	void erase() const
	{
		if(!--the_handle->cnt)
			release<Handle>(the_handle);
		the_handle = nil().the_handle;
		the_handle->cnt++;
	}
	int32 compare(const MutableString& arg) const
	{
		return ::strverscmp((char*)the_handle->data, (char*)arg.the_handle->data);
	}
	bool equal(const MutableString& arg) const
	{
		return !compare(arg);
	}
	const MutableString& operator=(const MutableString& arg) const
	{
		arg.the_handle->cnt++;
		if(!--the_handle->cnt)
			release<Handle>(the_handle);
		the_handle = arg.the_handle;
		return *this;
	}
	const MutableString& operator=(const char* arg) const
	{
		if(!--the_handle->cnt)
			release<Handle>(the_handle);
		uint32 size = ::strlen(arg);
		the_handle = clone((uint8*)arg, size);
		return *this;
	}
	bool operator==(const MutableString& arg) const
	{
		return (the_handle == arg.the_handle);
	}
	bool operator!=(const MutableString& arg) const
	{
		return (the_handle != arg.the_handle);
	}
	void copy_out(uint32& pos, Region& arg) const
	{
		copy_out(pos, arg.data, arg.size);
	}
	void copy_out(uint32& pos, uint8* arg, uint32 size) const
	{
		assert(the_handle->size >= pos + size);
		::memcpy(arg, the_handle->data + pos, size);
		pos += size;
	}
	void copy_in(uint32& pos, const MutableString& arg) const
	{
		copy_in(pos, arg.the_handle->data, arg.the_handle->size);
	}
	void copy_in(uint32& pos, Region& arg) const
	{
		copy_in(pos, arg.data, arg.size);
	}
	void copy_in(uint32& pos, uint8* arg, uint32 size) const
	{
		if(pos + size > the_handle->size)
		{
			uint32 granule = (((pos + size) / 0x40) + 1) * 0x40;
			the_handle = --the_handle->cnt
				? acquire<Handle>(sizeof(Handle) + sizeof(uint8) * granule)
				: acquire<Handle>(the_handle, sizeof(Handle) + sizeof(uint8) * granule);
			the_handle->cnt = 1;
			the_handle->size = granule;
		}
		::memcpy(the_handle->data + pos, arg, size);
		pos += size;
	}
	void copy_in(uint32& pos, const char* arg) const
	{
		uint32 size = ::strlen(arg);
		copy_in(pos, (uint8*)arg, size);
	}
	void copy_in(uint32& pos, bool arg) const
	{
		uint8 buffer[bool_max_digit];
		uint8* data = buffer;
		uint32 size = bool_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, int8 arg) const
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, int16 arg) const
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, int32 arg) const
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, int64 arg) const
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, uint8 arg) const
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, uint16 arg) const
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, uint32 arg) const
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, uint64 arg) const
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, float32 arg, uint8 precision = float32_precision) const
	{
		uint8 buffer[float32_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float32_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, float64 arg, uint8 precision = float64_precision) const
	{
		uint8 buffer[float64_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float64_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		copy_in(pos, data, size);
	}
	void copy_in(uint32& pos, float128 arg, uint8 precision = float128_precision) const
	{
		uint8 buffer[float128_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float128_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		copy_in(pos, data, size);
	}
	const MutableString& prepend(const MutableString& arg) const
	{
		return prepend(arg.the_handle->data, arg.the_handle->size);
	}
	const MutableString& prepend(Region& arg) const
	{
		return prepend(arg.data, arg.size);
	}
	const MutableString& prepend(uint8* arg, uint32 size) const
	{
		uint32 total = the_handle->size + size;
		Handle* handle = acquire<Handle>(sizeof(Handle) + sizeof(uint8) * total);
		handle->cnt = 1;
		handle->size = total;
		::memcpy(handle->data, arg, size);
		::memcpy(handle->data + size, the_handle->data, the_handle->size);
		handle->data[total] = '\0';
		if(!--the_handle->cnt)
			release<Handle>(the_handle);
		the_handle = handle;
		return *this;
	}
	const MutableString& prepend(const char* arg) const
	{
		uint32 size = ::strlen(arg);
		return prepend((uint8*)arg, size);
	}
	const MutableString& prepend(bool arg) const
	{
		uint8 buffer[bool_max_digit];
		uint8* data = buffer;
		uint32 size = bool_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(int8 arg) const
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(int16 arg) const
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(int32 arg) const
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(int64 arg) const
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(uint8 arg) const
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(uint16 arg) const
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(uint32 arg) const
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(uint64 arg) const
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		return prepend(data, size);
	}
	const MutableString& prepend(float32 arg, uint8 precision = float32_precision) const
	{
		uint8 buffer[float32_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float32_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		return prepend(data, size);
	}
	const MutableString& prepend(float64 arg, uint8 precision = float64_precision) const
	{
		uint8 buffer[float64_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float64_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		return prepend(data, size);
	}
	const MutableString& prepend(float128 arg, uint8 precision = float128_precision) const
	{
		uint8 buffer[float128_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float128_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		return prepend(data, size);
	}
	const MutableString& append(const MutableString& arg) const
	{
		if(the_handle == nil().the_handle)
		{
			the_handle->cnt--;
			arg.the_handle->cnt++;
			the_handle = arg.the_handle;
		}
		else
			append(arg.the_handle->data, arg.the_handle->size);
		return *this;
	}
	const MutableString& append(Region& arg) const
	{
		return append(arg.data, arg.size);
	}
	const MutableString& append(uint8* arg, uint32 size) const
	{
		uint32 total = the_handle->size + size;
		if(--the_handle->cnt)
		{
			Handle* handle = acquire<Handle>(sizeof(Handle) + sizeof(uint8) * total);
			::memcpy(handle->data, the_handle->data, the_handle->size);
			::memcpy(handle->data + the_handle->size, arg, size);
			the_handle = handle;
		}
		else
		{
			the_handle = acquire<Handle>(the_handle, sizeof(Handle) + sizeof(uint8) * total);
			::memcpy(the_handle->data + the_handle->size, arg, size);
		}
		the_handle->cnt = 1;
		the_handle->size = total;
		the_handle->data[total] = '\0';
		return *this;
	}
	const MutableString& append(const char* arg) const
	{
		uint32 size = ::strlen(arg);
		return append((uint8*)arg, size);
	}
	const MutableString& append(bool arg) const
	{
		uint8 buffer[bool_max_digit];
		uint8* data = buffer;
		uint32 size = bool_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(int8 arg) const
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(int16 arg) const
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(int32 arg) const
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(int64 arg) const
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(uint8 arg) const
	{
		uint8 buffer[int8_max_digit];
		uint8* data = buffer;
		uint32 size = int8_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(uint16 arg) const
	{
		uint8 buffer[int16_max_digit];
		uint8* data = buffer;
		uint32 size = int16_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(uint32 arg) const
	{
		uint8 buffer[int32_max_digit];
		uint8* data = buffer;
		uint32 size = int32_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(uint64 arg) const
	{
		uint8 buffer[int64_max_digit];
		uint8* data = buffer;
		uint32 size = int64_max_digit;
		Convert::it(data, size, arg);
		return append(data, size);
	}
	const MutableString& append(float32 arg, uint8 precision = float32_precision) const
	{
		uint8 buffer[float32_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float32_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		return append(data, size);
	}
	const MutableString& append(float64 arg, uint8 precision = float64_precision) const
	{
		uint8 buffer[float64_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float64_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		return append(data, size);
	}
	const MutableString& append(float128 arg, uint8 precision = float128_precision) const
	{
		uint8 buffer[float128_max_digit + int8_max];
		uint8* data = buffer;
		uint32 size = float128_max_digit + int8_max;
		Convert::it(data, size, arg, precision);
		return append(data, size);
	}
	const MutableString& attach(uint8 arg) const
	{
		return append((uint8*)&arg, sizeof(arg));
	}
	int8 to_int8() const
	{
		char* tail = 0;
		int8 arg = ::strtol((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : int8_nil;
	}
	int16 to_int16() const
	{
		char* tail = 0;
		int16 arg = ::strtol((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : int16_nil;
	}
	int32 to_int32() const
	{
		char* tail = 0;
		int32 arg = ::strtol((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : int32_nil;
	}
	int64 to_int64() const
	{
		char* tail = 0;
		int64 arg = ::strtoll((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : int64_nil;
	}
	uint8 to_uint8() const
	{
		char* tail = 0;
		uint8 arg = ::strtoul((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : uint8_nil;
	}
	uint16 to_uint16() const
	{
		char* tail = 0;
		uint16 arg = ::strtoul((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : uint16_nil;
	}
	uint32 to_uint32() const
	{
		char* tail = 0;
		uint32 arg = ::strtoul((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : uint32_nil;
	}
	uint64 to_uint64() const
	{
		char* tail = 0;
		uint64 arg = ::strtoull((char*)the_handle->data, &tail, 10);
		return (tail && !*tail) ? arg : uint64_nil;
	}
	float32 to_float32() const
	{
		char* tail = 0;
		float32 arg = ::strtof((char*)the_handle->data, &tail);
		return (tail && !*tail) ? arg : float32_nil;
	}
	float64 to_float64() const
	{
		char* tail = 0;
		float64 arg = ::strtod((char*)the_handle->data, &tail);
		return (tail && !*tail) ? arg : float64_nil;
	}
	float128 to_float128() const
	{
		char* tail = 0;
		float128 arg = ::strtold((char*)the_handle->data, &tail);
		return (tail && !*tail) ? arg : float128_nil;
	}
private:
	struct Handle
	{
		uint32 cnt;
		uint32 size;
		uint8 data[1];
	};
private:
	static Handle* clone(uint8* data, uint32 size)
	{
		Handle* handle = acquire<Handle>(sizeof(Handle) + sizeof(uint8) * size);
		handle->cnt = 1;
		handle->size = size;
		::memcpy(handle->data, data, size);
		handle->data[size] = '\0';
		return handle;
	}
private:
	mutable Handle* the_handle;
};

inline bool nil(const MutableString& arg)
{
	return arg == MutableString::nil();
}

inline bool is(const MutableString& arg)
{
	return arg != MutableString::nil();
}

typedef const MutableString String;

#define bs(value) core::String(value, sizeof(value))
#define cs(value) core::String((core::uint8*)value, sizeof(value) - 1)
#define rs(value) core::String(value.data, value.size)
#define cs_eng(value) cs(value)
#define cs_pl(value)

NAMESPACE_END(core);

#endif // CORE_STRING_HPP
