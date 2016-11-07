
const auto bool_max_digit = 5;
const auto int8_max_digit = 4;
const auto int16_max_digit = 6;
const auto int32_max_digit = 12;
const auto int64_max_digit = 22;
const auto int128_max_digit = 41;
const auto float32_max_digit = 22;
const auto float64_max_digit = 22;
const auto float128_max_digit = 22;
const auto float32_precision = 2;
const auto float64_precision = 4;
const auto float128_precision = 8;

enum Base
{
    decimal = 10,
    hexdecimal = 16
};

template<class Type>
inline Type absolute_int(Type var)
{
    return var * (((var > 0) << 1) - 1);
}

class Convert
{
public:
    static void it(uint8*& data, int64& size, bool arg)
    {
        static const int64 sizes[] = { 5, 4 };
        static const char* names[] = { "false", "true" };
        size = sizes[arg];
        ::memcpy(data, names[arg], size);
    }
    static void it(uint8*& data, int64& size, int8 arg)
    {
        data += size;
        auto end = data;
        auto fixed = absolute_int(arg);
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, int64& size, int16 arg)
    {
        data += size;
        auto end = data;
        auto fixed = absolute_int(arg);
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, int64& size, int32 arg)
    {
        data += size;
        auto end = data;
        auto fixed = absolute_int(arg);
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, int64& size, int64 arg)
    {
        data += size;
        auto end = data;
        auto fixed = absolute_int(arg);
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, int64& size, uint8 arg, Base base)
    {
        data += size;
        auto end = data;
        auto fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        }
        while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, int64& size, uint16 arg, Base base)
    {
        data += size;
        auto end = data;
        auto fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        }
        while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, int64& size, uint32 arg, Base base)
    {
        data += size;
        auto end = data;
        auto fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        }
        while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, int64& size, uint64 arg, Base base)
    {
        data += size;
        auto end = data;
        auto fixed = arg;
        do
        {
            *--data = byte2digit(fixed % base);
        }
        while(fixed /= base);
        size = end - data;
    }
    static void it(uint8*& data, int64& size, float32 arg, int64 precision)
    {
        data += size;
        auto end = data;
        float64 ffixed = arg * ::pow(10, precision);
        int64 fixed = ::llround(ffixed);
        while(precision--)
        {
            *--data = fixed % 10 + '0';
            fixed /= 10;
        }
        *--data = '.';
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, int64& size, float64 arg, int64 precision)
    {
        data += size;
        auto end = data;
        float128 ffixed = arg * ::pow(10, precision);
        int64 fixed = ::llroundl(ffixed);
        while(precision--)
        {
            *--data = fixed % 10 + '0';
            fixed /= 10;
        }
        *--data = '.';
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
    static void it(uint8*& data, int64& size, float128 arg, int64 precision)
    {
        float128 integer, fraction;
        fraction = ::modfl(::fabsl(arg), &integer);
        fraction *= ::pow(10, precision);
        data += size;
        auto end = data;
        int64 fixed = ::llroundl(fraction);
        while(precision--)
        {
            *--data = fixed % 10 + '0';
            fixed /= 10;
        }
        *--data = '.';
        fixed += ::llroundl(integer);
        do
        {
            *--data = fixed % 10 + '0';
        }
        while(fixed /= 10);
        if(arg < 0)
            *--data = '-';
        size = end - data;
    }
private:
    static uint8 byte2digit(uint8 byte)
    {
        static const uint8 byte2digit[] = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'A', 'B', 'C', 'D', 'E', 'F'
        };
        return byte2digit[byte];
    }
};
