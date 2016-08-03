
const uint8 bool_max_digit = 5;
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

enum Base
{
    decimal = 10,
    hexdecimal = 16
};

class Convert
{
public:
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
        int8 fixed = ::abs(arg);
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
        if(signbit(arg))
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, int16 arg)
    {
        data += size;
        uint8* end = data;
        int16 fixed = ::abs(arg);
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
        if(signbit(arg))
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, int32 arg)
    {
        data += size;
        uint8* end = data;
        int32 fixed = ::abs(arg);
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
        if(signbit(arg))
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, int64 arg)
    {
        data += size;
        uint8* end = data;
        int64 fixed = ::llabs(arg);
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
        if(signbit(arg))
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, uint8 arg, Base base)
    {
        data += size;
        uint8* end = data;
        uint8 fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        } while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, uint16 arg, Base base)
    {
        data += size;
        uint8* end = data;
        uint16 fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        } while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, uint32 arg, Base base)
    {
        data += size;
        uint8* end = data;
        uint32 fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        } while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, uint64 arg, Base base)
    {
        data += size;
        uint8* end = data;
        uint64 fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        } while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, float32 arg, uint8 precision)
    {
        data += size;
        uint8* end = data;
        float64 ffixed = arg * ::pow(10, precision);
        int64 fixed = ::llround(ffixed);
        for(uint32 i = 0; i < precision; i++)
        {
            *--data = fixed % 10 + '0';
            fixed /= 10;
        }
        *--data = '.';
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
        if(signbit(arg))
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, uint32& size, float64 arg, uint8 precision)
    {
        data += size;
        uint8* end = data;
        float128 ffixed = arg * ::pow(10, precision);
        int64 fixed = ::llroundl(ffixed);
        for(uint32 i = 0; i < precision; i++)
        {
            *--data = fixed % 10 + '0';
            fixed /= 10;
        }
        *--data = '.';
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
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
        int64 fixed = ::llroundl(fraction);
        for(uint32 i = 0; i < precision; i++)
        {
            *--data = fixed % 10 + '0';
            fixed /= 10;
        }
        *--data = '.';
        fixed += ::llroundl(integer);
        do
        {
            *--data = fixed % 10 + '0';
        } while(fixed /= 10);
        if(signbit(arg))
            *--data = '-';
        size = end - data;
    }
private:
    static core::uint8 byte2digit(core::uint8 byte)
    {
        static const core::uint8 byte2digit[] = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'A', 'B', 'C', 'D', 'E', 'F'
        };
        return byte2digit[byte];
    }
};
