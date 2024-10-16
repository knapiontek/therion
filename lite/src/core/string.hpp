
typedef const class StringClass String;

class StringClass
{
    struct Handle;
public:
    typedef Shared<String> share;
    typedef Managed<String> manage;
public:
    class Iterator
    {
        friend class StringClass;
    public:
        bool is_head()
        {
            return the_pos < the_handle->data;
        }
        bool is_tail()
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
        int64 position()
        {
            return the_pos - the_handle->data;
        }
        uint8& value()
        {
            return *the_pos;
        }
        bool operator!=(Iterator& it)
        {
            return the_pos != it.the_pos;
        }
        void operator++()
        {
            the_pos++;
        }
        void operator--()
        {
            the_pos--;
        }
        Iterator& operator*()
        {
            return *this;
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
    class Reverse : public Iterator
    {
        friend class StringClass;
    public:
        void operator++()
        {
            Iterator::operator--();
        }
    private:
        Reverse(Iterator&& it) : Iterator(it)
        {

        }
    };
public:
    StringClass()
    {
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    StringClass(Nil)
    {
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    StringClass(String& arg)
    {
        the_handle = arg.the_handle;
        the_handle->cnt++;
    }
    StringClass(const char* arg)
    {
        auto size = ::strlen(arg);
        the_handle = clone((uint8*)arg, size);
    }
    explicit StringClass(uint8* arg, int64 size)
    {
        the_handle = clone(arg, size);
    }
    explicit StringClass(bool arg)
    {
        uint8 buffer[bool_max_digit];
        auto data = buffer;
        int64 size = bool_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit StringClass(int8 arg)
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit StringClass(int16 arg)
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit StringClass(int32 arg)
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit StringClass(int64 arg)
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit StringClass(uint8 arg, Base base = decimal)
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit StringClass(uint16 arg, Base base = decimal)
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit StringClass(uint32 arg, Base base = decimal)
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit StringClass(uint64 arg, Base base = decimal)
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit StringClass(float32 arg, int64 precision = float32_precision)
    {
        uint8 buffer[float32_max_digit + int8_max];
        auto data = buffer;
        int64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        the_handle = clone(data, size);
    }
    explicit StringClass(float64 arg, int64 precision = float64_precision)
    {
        uint8 buffer[float64_max_digit + int8_max];
        auto data = buffer;
        int64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        the_handle = clone(data, size);
    }
    explicit StringClass(float128 arg, int64 precision = float128_precision)
    {
        uint8 buffer[float128_max_digit + int8_max];
        auto data = buffer;
        int64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        the_handle = clone(data, size);
    }
    ~StringClass()
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
    Iterator begin() const
    {
        return Iterator(the_handle->data, the_handle);
    }
    Iterator end() const
    {
        return tail();
    }
    Reverse rbegin() const
    {
        return Iterator(the_handle->data + the_handle->size - 1, the_handle);
    }
    Reverse rend() const
    {
        return head();
    }
    uint8* data() const
    {
        return the_handle->data;
    }
    char* ascii() const
    {
        return (char*)the_handle->data;
    }
    uint8& at(int64 pos) const
    {
        certify(pos < the_handle->size);
        return the_handle->data[pos];
    }
    uint8& operator[](int64 pos) const
    {
        return at(pos);
    }
    int64 size() const
    {
        return the_handle->size;
    }
    void size(int64 size) const
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
    StringClass clone() const
    {
        return StringClass(the_handle->data, the_handle->size);
    }
    void erase() const
    {
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    int32 compare(String& arg) const
    {
        return ::strverscmp((char*)the_handle->data, (char*)arg.the_handle->data);
    }
    int32 compare(const char* arg) const
    {
        return ::strverscmp((char*)the_handle->data, arg);
    }
    bool equal(String& arg) const
    {
        return !compare(arg);
    }
    bool equal(const char* arg) const
    {
        return !compare(arg);
    }
    String& operator=(Nil) const
    {
        nil_handle()->cnt++;
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        the_handle = nil_handle();
        return *this;
    }
    String& operator=(String& arg) const
    {
        arg.the_handle->cnt++;
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        the_handle = arg.the_handle;
        return *this;
    }
    String& operator=(const char* arg) const
    {
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        auto size = ::strlen(arg);
        the_handle = clone((uint8*)arg, size);
        return *this;
    }
    friend bool operator==(Nil, String& arg)
    {
        return (nil_handle() == arg.the_handle);
    }
    friend bool operator!=(Nil, String& arg)
    {
        return (nil_handle() != arg.the_handle);
    }
    bool operator==(Nil) const
    {
        return (the_handle == nil_handle());
    }
    bool operator!=(Nil) const
    {
        return (the_handle != nil_handle());
    }
    bool operator==(String& arg) const
    {
        return (the_handle == arg.the_handle);
    }
    bool operator!=(String& arg) const
    {
        return (the_handle != arg.the_handle);
    }
    void copy_out(int64& pos, uint8* arg, int64 size) const
    {
        certify(the_handle->size >= pos + size);
        ::memcpy(arg, the_handle->data + pos, size);
        pos += size;
    }
    void copy_in(int64& pos, String& arg) const
    {
        copy_in(pos, arg.the_handle->data, arg.the_handle->size);
    }
    void copy_in(int64& pos, const char* arg) const
    {
        auto size = ::strlen(arg);
        copy_in(pos, (uint8*)arg, size);
    }
    void copy_in(int64& pos, uint8* arg, int64 size) const
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
    void copy_in(int64& pos, bool arg) const
    {
        uint8 buffer[bool_max_digit];
        auto data = buffer;
        int64 size = bool_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, int8 arg) const
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, int16 arg) const
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, int32 arg) const
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, int64 arg) const
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, uint8 arg, Base base = decimal) const
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, uint16 arg, Base base = decimal) const
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, uint32 arg, Base base = decimal) const
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, uint64 arg, Base base = decimal) const
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, float32 arg, int64 precision = float32_precision) const
    {
        uint8 buffer[float32_max_digit + int8_max];
        auto data = buffer;
        int64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, float64 arg, int64 precision = float64_precision) const
    {
        uint8 buffer[float64_max_digit + int8_max];
        auto data = buffer;
        int64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        copy_in(pos, data, size);
    }
    void copy_in(int64& pos, float128 arg, int64 precision = float128_precision) const
    {
        uint8 buffer[float128_max_digit + int8_max];
        auto data = buffer;
        int64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        copy_in(pos, data, size);
    }
    String& prepend(String& arg) const
    {
        return prepend(arg.the_handle->data, arg.the_handle->size);
    }
    String& prepend(const char* arg) const
    {
        auto size = ::strlen(arg);
        return prepend((uint8*)arg, size);
    }
    String& prepend(uint8* arg, int64 size) const
    {
        auto total = the_handle->size + size;
        auto handle = acquire<Handle>(sizeof(Handle) + sizeof(uint8) * total);
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
    String& prepend(bool arg) const
    {
        uint8 buffer[bool_max_digit];
        auto data = buffer;
        int64 size = bool_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    String& prepend(int8 arg) const
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    String& prepend(int16 arg) const
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    String& prepend(int32 arg) const
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    String& prepend(int64 arg) const
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    String& prepend(uint8 arg, Base base = decimal) const
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    String& prepend(uint16 arg, Base base = decimal) const
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    String& prepend(uint32 arg, Base base = decimal) const
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    String& prepend(uint64 arg, Base base = decimal) const
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    String& prepend(float32 arg, int64 precision = float32_precision) const
    {
        uint8 buffer[float32_max_digit + int8_max];
        auto data = buffer;
        int64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return prepend(data, size);
    }
    String& prepend(float64 arg, int64 precision = float64_precision) const
    {
        uint8 buffer[float64_max_digit + int8_max];
        auto data = buffer;
        int64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return prepend(data, size);
    }
    String& prepend(float128 arg, int64 precision = float128_precision) const
    {
        uint8 buffer[float128_max_digit + int8_max];
        auto data = buffer;
        int64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return prepend(data, size);
    }
    String& append(String& arg) const
    {
        if(the_handle == nil_handle())
        {
            the_handle->cnt--;
            the_handle = arg.the_handle;
            the_handle->cnt++;
        }
        else
            append(arg.the_handle->data, arg.the_handle->size);
        return *this;
    }
    String& append(const char* arg) const
    {
        auto size = ::strlen(arg);
        append((uint8*)arg, size);
        return *this;
    }
    String& append(uint8* arg, int64 size) const
    {
        auto total = the_handle->size + size;
        if(--the_handle->cnt)
        {
            auto handle = acquire<Handle>(sizeof(Handle) + sizeof(uint8) * total);
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
    String& append(bool arg) const
    {
        uint8 buffer[bool_max_digit];
        auto data = buffer;
        int64 size = bool_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    String& append(int8 arg) const
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    String& append(int16 arg) const
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    String& append(int32 arg) const
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    String& append(int64 arg) const
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    String& append(uint8 arg, Base base = decimal) const
    {
        uint8 buffer[int8_max_digit];
        auto data = buffer;
        int64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    String& append(uint16 arg, Base base = decimal) const
    {
        uint8 buffer[int16_max_digit];
        auto data = buffer;
        int64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    String& append(uint32 arg, Base base = decimal) const
    {
        uint8 buffer[int32_max_digit];
        auto data = buffer;
        int64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    String& append(uint64 arg, Base base = decimal) const
    {
        uint8 buffer[int64_max_digit];
        auto data = buffer;
        int64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    String& append(float32 arg, int64 precision = float32_precision) const
    {
        uint8 buffer[float32_max_digit + int8_max];
        auto data = buffer;
        int64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return append(data, size);
    }
    String& append(float64 arg, int64 precision = float64_precision) const
    {
        uint8 buffer[float64_max_digit + int8_max];
        auto data = buffer;
        int64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return append(data, size);
    }
    String& append(float128 arg, int64 precision = float128_precision) const
    {
        uint8 buffer[float128_max_digit + int8_max];
        auto data = buffer;
        int64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return append(data, size);
    }
    String& attach(uint8 arg) const
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
        int16 cnt;
        int32 size;
        uint8 data[1];
    };
private:
    static Handle* nil_handle()
    {
        static Handle nil = { 1, 0, { 0 } };
        return &nil;
    }
    static Handle* clone(uint8* data, int64 size)
    {
        auto handle = acquire<Handle>(sizeof(Handle) + sizeof(uint8) * size);
        handle->cnt = 1;
        handle->size = size;
        ::memcpy(handle->data, data, size);
        handle->data[size] = '\0';
        return handle;
    }
private:
    mutable Handle* the_handle;
};
