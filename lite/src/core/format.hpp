
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
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = arg.data();
        item.size = arg.size();
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint8* arg, uint32 size)
    {
        assert(arg && (the_cnt < (sizeof(the_items) / sizeof(Item))));
        Item& item = the_items[the_cnt++];
        item.pt = arg;
        item.size = size;
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(bool arg)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int8 arg)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int16 arg)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int32 arg)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(int64 arg)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint8 arg, Base base = decimal)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint16 arg, Base base = decimal)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint32 arg, Base base = decimal)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(uint64 arg, Base base = decimal)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, base);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(float32 arg, uint8 precision = float32_precision)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, precision);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(float64 arg, uint8 precision = float64_precision)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, precision);
        return *(FormatReturn*)this;
    }
    FormatReturn& arg(float128 arg, uint8 precision = float128_precision)
    {
        assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
        Item& item = the_items[the_cnt++];
        item.pt = item.data;
        item.size = sizeof(item.data);
        Convert::it(item.pt, item.size, arg, precision);
        return *(FormatReturn*)this;
    }
private:
    String& merge(bool new_line)
    {
        static String cs_new_line = cs("\n");
        auto max = uint8_max;
        static const uint8 hex2index[] =    {
            0, 1, 2, 3, 4, 5, 6, 7, 8,
            max, max, max, max, max, max, max,
            9,
            0xA, 0xB, 0xC, 0xD, 0xE
        };

        uint32 pos = 0;
        uint32 last = 0;
        if(new_line)
            the_result.copy_in(pos, cs_new_line);
        String::Iterator it = the_format.head();
        while(it.next())
        {
            if('$' == it.value())
            {
                the_result.copy_in(pos, the_format.data() + last, it.position() - last);
                last = it.position() + 1; // skip %
                if(it.next() && '$' != it.value())
                {
                    last++; // skip digit
                    uint32 index = it.value() - '1';
                    verify(index < sizeof(hex2index));
                    index = hex2index[index];
                    assert(index < the_cnt);
                    Item& item = the_items[index];
                    the_result.copy_in(pos, item.pt, item.size);
                }
            }
        }

        the_result.copy_in(pos, the_format.data() + last, it.position() - last);
        the_result.size(pos);
        return the_result;
    }
    String& join(bool new_line)
    {
        static String cs_new_line = cs("\n");
        uint32 size = new_line;
        for(uint32 i = 0; i < the_cnt; i++)
            size += the_items[i].size;
        the_result.size(size);

        uint32 pos = 0;
        if(new_line)
            the_result.copy_in(pos, cs_new_line);
        for(uint32 i = 0; i < the_cnt; i++)
        {
            Item& item = the_items[i];
            the_result.copy_in(pos, item.pt, item.size);
        }

        assert(size == pos);
        return the_result;
    }
private:
    String the_format;
    String the_result;
    uint32 the_cnt;
    struct Item
    {
        uint8 data[int8_max];
        uint8* pt;
        uint32 size;
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
