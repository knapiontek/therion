
class Mutable
{
    struct Handle;
public:
    typedef Shared<const Mutable> share;
    typedef Managed<const Mutable> manage;
public:
    class Iterator
    {
        friend class Mutable;
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
        uint64 position()
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
    protected:
        uint8* the_pos;
    private:
        Handle* the_handle;
    };
    class LoopIterator : public Iterator
    {
        friend class Mutable;
    public:
        bool operator!=(LoopIterator& it)
        {
            return the_pos != it.the_pos;
        }
        void operator++()
        {
            the_pos++;
        }
        LoopIterator& operator*()
        {
            return *this;
        }
    private:
        LoopIterator(Iterator& it) : Iterator(it)
        {

        }
        LoopIterator(Iterator&& it) : Iterator(it)
        {

        }
    };
    class ReverseLoopIterator : public LoopIterator
    {
        friend class Mutable;
    public:
        void operator++()
        {
            the_pos--;
        }
    private:
        ReverseLoopIterator(Iterator& it) : LoopIterator(it)
        {

        }
        ReverseLoopIterator(Iterator&& it) : LoopIterator(it)
        {

        }
    };
public:
    Mutable()
    {
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    Mutable(const Nil&)
    {
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    Mutable(const Mutable& arg)
    {
        the_handle = arg.the_handle;
        the_handle->cnt++;
    }
    Mutable(const char* arg)
    {
        uint64 size = ::strlen(arg);
        the_handle = clone((uint8*)arg, size);
    }
    explicit Mutable(uint8* arg, uint64 size)
    {
        the_handle = clone(arg, size);
    }
    explicit Mutable(bool arg)
    {
        uint8 buffer[bool_max_digit];
        uint8* data = buffer;
        uint64 size = bool_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit Mutable(int8 arg)
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit Mutable(int16 arg)
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit Mutable(int32 arg)
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit Mutable(int64 arg)
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg);
        the_handle = clone(data, size);
    }
    explicit Mutable(uint8 arg, Base base = decimal)
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit Mutable(uint16 arg, Base base = decimal)
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit Mutable(uint32 arg, Base base = decimal)
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit Mutable(uint64 arg, Base base = decimal)
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        the_handle = clone(data, size);
    }
    explicit Mutable(float32 arg, uint8 precision = float32_precision)
    {
        uint8 buffer[float32_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        the_handle = clone(data, size);
    }
    explicit Mutable(float64 arg, uint8 precision = float64_precision)
    {
        uint8 buffer[float64_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        the_handle = clone(data, size);
    }
    explicit Mutable(float128 arg, uint8 precision = float128_precision)
    {
        uint8 buffer[float128_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        the_handle = clone(data, size);
    }
    ~Mutable()
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
    LoopIterator begin() const
    {
        return LoopIterator(Iterator(the_handle->data, the_handle));
    }
    LoopIterator end() const
    {
        return LoopIterator(tail());
    }
    ReverseLoopIterator rbegin() const
    {
        return ReverseLoopIterator(Iterator(the_handle->data + the_handle->size - 1, the_handle));
    }
    ReverseLoopIterator rend() const
    {
        return ReverseLoopIterator(head());
    }
    uint8* data() const
    {
        return the_handle->data;
    }
    char* ascii() const
    {
        return (char*)the_handle->data;
    }
    uint8& at(uint64 pos) const
    {
        assert(pos < the_handle->size);
        return the_handle->data[pos];
    }
    uint8& operator[](uint64 pos) const
    {
        return at(pos);
    }
    uint64 size() const
    {
        return the_handle->size;
    }
    void size(uint64 size) const
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
    Mutable clone() const
    {
        return Mutable(the_handle->data, the_handle->size);
    }
    void erase() const
    {
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    int32 compare(const Mutable& arg) const
    {
        return ::strverscmp((char*)the_handle->data, (char*)arg.the_handle->data);
    }
    int32 compare(const char* arg) const
    {
        return ::strverscmp((char*)the_handle->data, arg);
    }
    bool equal(const Mutable& arg) const
    {
        return !compare(arg);
    }
    bool equal(const char* arg) const
    {
        return !compare(arg);
    }
    const Mutable& operator=(const Nil&) const
    {
        nil_handle()->cnt++;
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        the_handle = nil_handle();
        return *this;
    }
    const Mutable& operator=(const Mutable& arg) const
    {
        arg.the_handle->cnt++;
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        the_handle = arg.the_handle;
        return *this;
    }
    const Mutable& operator=(const char* arg) const
    {
        if(!--the_handle->cnt)
            release<Handle>(the_handle);
        uint64 size = ::strlen(arg);
        the_handle = clone((uint8*)arg, size);
        return *this;
    }
    friend bool operator==(const Nil&, const Mutable& arg)
    {
        return (nil_handle() == arg.the_handle);
    }
    friend bool operator!=(const Nil&, const Mutable& arg)
    {
        return (nil_handle() != arg.the_handle);
    }
    bool operator==(const Nil&) const
    {
        return (the_handle == nil_handle());
    }
    bool operator!=(const Nil&) const
    {
        return (the_handle != nil_handle());
    }
    bool operator==(const Mutable& arg) const
    {
        return (the_handle == arg.the_handle);
    }
    bool operator!=(const Mutable& arg) const
    {
        return (the_handle != arg.the_handle);
    }
    void copy_out(uint64& pos, uint8* arg, uint64 size) const
    {
        assert(the_handle->size >= pos + size);
        ::memcpy(arg, the_handle->data + pos, size);
        pos += size;
    }
    void copy_in(uint64& pos, const Mutable& arg) const
    {
        copy_in(pos, arg.the_handle->data, arg.the_handle->size);
    }
    void copy_in(uint64& pos, const char* arg) const
    {
        uint64 size = ::strlen(arg);
        copy_in(pos, (uint8*)arg, size);
    }
    void copy_in(uint64& pos, uint8* arg, uint64 size) const
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
    void copy_in(uint64& pos, bool arg) const
    {
        uint8 buffer[bool_max_digit];
        uint8* data = buffer;
        uint64 size = bool_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, int8 arg) const
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, int16 arg) const
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, int32 arg) const
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, int64 arg) const
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, uint8 arg, Base base = decimal) const
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, uint16 arg, Base base = decimal) const
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, uint32 arg, Base base = decimal) const
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, uint64 arg, Base base = decimal) const
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, float32 arg, uint8 precision = float32_precision) const
    {
        uint8 buffer[float32_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, float64 arg, uint8 precision = float64_precision) const
    {
        uint8 buffer[float64_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        copy_in(pos, data, size);
    }
    void copy_in(uint64& pos, float128 arg, uint8 precision = float128_precision) const
    {
        uint8 buffer[float128_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        copy_in(pos, data, size);
    }
    const Mutable& prepend(const Mutable& arg) const
    {
        return prepend(arg.the_handle->data, arg.the_handle->size);
    }
    const Mutable& prepend(const char* arg) const
    {
        uint64 size = ::strlen(arg);
        return prepend((uint8*)arg, size);
    }
    const Mutable& prepend(uint8* arg, uint64 size) const
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
    const Mutable& prepend(bool arg) const
    {
        uint8 buffer[bool_max_digit];
        uint8* data = buffer;
        uint64 size = bool_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    const Mutable& prepend(int8 arg) const
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    const Mutable& prepend(int16 arg) const
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    const Mutable& prepend(int32 arg) const
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    const Mutable& prepend(int64 arg) const
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg);
        return prepend(data, size);
    }
    const Mutable& prepend(uint8 arg, Base base = decimal) const
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    const Mutable& prepend(uint16 arg, Base base = decimal) const
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    const Mutable& prepend(uint32 arg, Base base = decimal) const
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    const Mutable& prepend(uint64 arg, Base base = decimal) const
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        return prepend(data, size);
    }
    const Mutable& prepend(float32 arg, uint8 precision = float32_precision) const
    {
        uint8 buffer[float32_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return prepend(data, size);
    }
    const Mutable& prepend(float64 arg, uint8 precision = float64_precision) const
    {
        uint8 buffer[float64_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return prepend(data, size);
    }
    const Mutable& prepend(float128 arg, uint8 precision = float128_precision) const
    {
        uint8 buffer[float128_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return prepend(data, size);
    }
    const Mutable& append(const Mutable& arg) const
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
    const Mutable& append(const char* arg) const
    {
        uint64 size = ::strlen(arg);
        append((uint8*)arg, size);
        return *this;
    }
    const Mutable& append(uint8* arg, uint64 size) const
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
    const Mutable& append(bool arg) const
    {
        uint8 buffer[bool_max_digit];
        uint8* data = buffer;
        uint64 size = bool_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    const Mutable& append(int8 arg) const
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    const Mutable& append(int16 arg) const
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    const Mutable& append(int32 arg) const
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    const Mutable& append(int64 arg) const
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg);
        return append(data, size);
    }
    const Mutable& append(uint8 arg, Base base = decimal) const
    {
        uint8 buffer[int8_max_digit];
        uint8* data = buffer;
        uint64 size = int8_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    const Mutable& append(uint16 arg, Base base = decimal) const
    {
        uint8 buffer[int16_max_digit];
        uint8* data = buffer;
        uint64 size = int16_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    const Mutable& append(uint32 arg, Base base = decimal) const
    {
        uint8 buffer[int32_max_digit];
        uint8* data = buffer;
        uint64 size = int32_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    const Mutable& append(uint64 arg, Base base = decimal) const
    {
        uint8 buffer[int64_max_digit];
        uint8* data = buffer;
        uint64 size = int64_max_digit;
        Convert::it(data, size, arg, base);
        return append(data, size);
    }
    const Mutable& append(float32 arg, uint8 precision = float32_precision) const
    {
        uint8 buffer[float32_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float32_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return append(data, size);
    }
    const Mutable& append(float64 arg, uint8 precision = float64_precision) const
    {
        uint8 buffer[float64_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float64_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return append(data, size);
    }
    const Mutable& append(float128 arg, uint8 precision = float128_precision) const
    {
        uint8 buffer[float128_max_digit + int8_max];
        uint8* data = buffer;
        uint64 size = float128_max_digit + int8_max;
        Convert::it(data, size, arg, precision);
        return append(data, size);
    }
    const Mutable& attach(uint8 arg) const
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
        uint64 size;
        uint8 data[1];
    };
private:
    static Handle* nil_handle()
    {
        static Handle nil = { 1, 0, { 0 } };
        return &nil;
    }
    static Handle* clone(uint8* data, uint64 size)
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

typedef const Mutable String;
