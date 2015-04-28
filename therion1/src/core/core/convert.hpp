
#ifndef CORE_CONVERT_HPP
#define CORE_CONVERT_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

const uint8 bool_max_digit = 5; // uint8 allowes buffer[uint8_max]
const uint8 int8_max_digit = 4;
const uint8 int16_max_digit = 6;
const uint8 int32_max_digit = 12;
const uint8 int64_max_digit = 22;
const uint8 float32_max_digit = 22;
const uint8 float64_max_digit = 22;
const uint8 float128_max_digit = 22;
const uint8 float32_precision = 2;
const uint8 float64_precision = 4;
const uint8 float128_precision = 8;

struct Convert
{
	static void it(uint8*& data, uint32& size, bool arg)
	{
		static const uint32 sizes[] = { 5, 4 };
		static const char* names[] = { "false", "true" };
		size = sizes[arg];
		::memcpy(data, names[arg], size);
	}
	static void it(uint8*& data, uint32& size, int8 arg)
	{
		data += size;
		uint8* end = data;
		int8 decimal = ::abs(arg);
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, int16 arg)
	{
		data += size;
		uint8* end = data;
		int16 decimal = ::abs(arg);
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, int32 arg)
	{
		data += size;
		uint8* end = data;
		int32 decimal = ::abs(arg);
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, int64 arg)
	{
		data += size;
		uint8* end = data;
		int64 decimal = ::llabs(arg);
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, uint8 arg)
	{
		data += size;
		uint8* end = data;
		uint8 decimal = arg;
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, uint16 arg)
	{
		data += size;
		uint8* end = data;
		uint16 decimal = arg;
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, uint32 arg)
	{
		data += size;
		uint8* end = data;
		uint32 decimal = arg;
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, uint64 arg)
	{
		data += size;
		uint8* end = data;
		uint64 decimal = arg;
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, float32 arg, uint8 precision)
	{
		data += size;
		uint8* end = data;
		float64 fdecimal = arg * ::pow(10, precision);
		int64 decimal = ::llround(fdecimal);
		for(uint32 i = 0; i < precision; i++)
		{
			*--data = decimal % 10 + '0';
			decimal /= 10;
		}
		*--data = '.';
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, float64 arg, uint8 precision)
	{
		data += size;
		uint8* end = data;
		float128 fdecimal = arg * ::pow(10, precision);
		int64 decimal = ::llroundl(fdecimal);
		for(uint32 i = 0; i < precision; i++)
		{
			*--data = decimal % 10 + '0';
			decimal /= 10;
		}
		*--data = '.';
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
	static void it(uint8*& data, uint32& size, float128 arg, uint8 precision)
	{
		float128 integer, fraction;
		fraction = ::modfl(::fabsl(arg), &integer);
		fraction *= ::pow(10, precision);
		data += size;
		uint8* end = data;
		int64 decimal = ::llroundl(fraction);
		for(uint32 i = 0; i < precision; i++)
		{
			*--data = decimal % 10 + '0';
			decimal /= 10;
		}
		*--data = '.';
		decimal += ::llroundl(integer);
		do
		{
			*--data = decimal % 10 + '0';
		} while(decimal /= 10);
		if(signbit(arg))
			*--data = '-';
		size = end - data;
	}
};

NAMESPACE_END(core);

#endif // CORE_CONVERT_HPP
