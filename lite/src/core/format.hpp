
template<class FormatReturn>
class FormatClass
{
public:
    FormatClass() : the_cnt(0)
    {

    }
    FormatClass(String& format) : the_format(format), the_cnt(0)
    {

    }
    void format(String& format)
    {
        the_format = format;
        the_result = nil;
        the_cnt = 0;
    }
    String& end()
    {
        return the_format.is_empty()
            ? join(false)
            : merge(false);
    }
    String& endln()
    {
        return the_format.is_empty()
            ? join(true)
            : merge(true);
    }
    FormatReturn& arg(String& arg)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = arg.data();
        item.size = arg.size();
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(const char* arg)
    {
        certify(arg && (the_cnt < int64(sizeof(the_items) / sizeof(Item))));
        auto size = ::strlen(arg);
        auto& item = the_items[the_cnt++];
        item.pt = (uint8*)arg;
        item.size = size;
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint8* arg, int64 size)
    {
        certify(arg && (the_cnt < int64(sizeof(the_items) / sizeof(Item))));
        auto& item = the_items[the_cnt++];
        item.pt = arg;
        item.size = size;
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(bool arg)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int8 arg)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int16 arg)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int32 arg)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int64 arg)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint8 arg, Base base = decimal)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint16 arg, Base base = decimal)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint32 arg, Base base = decimal)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint64 arg, Base base = decimal)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(float32 arg, int64 precision = float32_precision)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, precision);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(float64 arg, int64 precision = float64_precision)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, precision);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(float128 arg, int64 precision = float128_precision)
    {
        certify(the_cnt < int64(sizeof(the_items) / sizeof(Item)));
        auto& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, precision);
        return *(FormatReturn*)this;
    }
private:
    String& merge(bool new_line)
    {
        static String cs_line = "\n";
        auto max = uint8_max;
        static const uint8 hex2index[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8,
            max, max, max, max, max, max, max,
            9,
            0xA, 0xB, 0xC, 0xD, 0xE
        };

        int64 pos = 0;
        auto last = 0;
        if(new_line)
            the_result.copy_in(pos, cs_line);
        for(auto& it : the_format)
        {
            if('$' == it.value())
            {
                the_result.copy_in(pos, the_format.data() + last, it.position() - last);
                last = it.position() + 1; // skip $
                if(it.next() && '$' != it.value())
                {
                    last++; // skip digit
                    auto index = it.value() - '1';
                    verify(index < int64(sizeof(hex2index)));
                    index = hex2index[index];
                    certify(index < the_cnt);
                    auto& item = the_items[index];
                    the_result.copy_in(pos, item.pt, item.size);
                }
            }
        }

        the_result.copy_in(pos, the_format.data() + last, the_format.size() - last);
        the_result.size(pos);
        return the_result;
    }
    String& join(bool new_line)
    {
        static String cs_line = "\n";
        int64 size = new_line;
        for(auto& i : range(the_cnt))
            size += the_items[i].size;
        the_result.size(size);

        int64 pos = 0;
        if(new_line)
            the_result.copy_in(pos, cs_line);
        for(auto& i : range(the_cnt))
        {
            auto& item = the_items[i];
            the_result.copy_in(pos, item.pt, item.size);
        }

        certify(size == pos);
        return the_result;
    }
private:
    String the_format;
    String the_result;
    int64 the_cnt;
    struct Item
    {
        uint8 data[int8_max];
        uint8* pt;
        int64 size;
    } the_items[0xF]; // hex 1 - F
};

class Format : public FormatClass<Format>
{
public:
    Format()
    {

    }
    Format(String& arg) : FormatClass<Format>(arg)
    {

    }
};
