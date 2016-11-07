
template<class Type>
class Managed
{
    template<class Other>
    friend class Managed;
public:
    Managed()
    {
        the_handle = acquire<Handle>(sizeof(Handle));
        the_handle->cnt = 1;
        new((void*)the_handle->var) Type();
    }
    Managed(const Nil&)
    {
        the_handle = nil_handle();
        the_handle->cnt++;
    }
    Managed(const Type& var)
    {
        the_handle = acquire<Handle>(sizeof(Handle));
        the_handle->cnt = 1;
        new((void*)the_handle->var) Type(var);
    }
    Managed(const Managed& arg)
    {
        the_handle = arg.the_handle;
        the_handle->cnt++;
    }
    template<class Other>
    Managed(const Managed<Other>& arg)
    {
        certify(static_cast<Type*>(arg.the_handle->var));
        the_handle = reinterpret_cast<Handle*>(arg.the_handle);
        the_handle->cnt++;
    }
    template<class... Args>
    Managed(const Args&... args)
    {
        the_handle = acquire<Handle>(sizeof(Handle));
        the_handle->cnt = 1;
        new((void*)the_handle->var) Type(args...);
    }
    ~Managed()
    {
        if(!--the_handle->cnt)
        {
            auto var = the_handle->var;
            var->~Type();
            release<Handle>(the_handle);
        }
    }
    Managed& operator=(const Nil&)
    {
        nil_handle()->cnt++;
        if(!--the_handle->cnt)
        {
            auto var = the_handle->var;
            var->~Type();
            release<Handle>(the_handle);
        }
        the_handle = nil_handle();
        return *this;
    }
    Managed& operator=(const Type& var)
    {
        certify(the_handle != nil_handle());
        *the_handle->var = var;
        return *this;
    }
    Managed& operator=(const Managed& arg)
    {
        arg.the_handle->cnt++;
        if(!--the_handle->cnt)
        {
            auto var = the_handle->var;
            var->~Type();
            release<Handle>(the_handle);
        }
        the_handle = arg.the_handle;
        return *this;
    }
    template<class Other>
    Managed& operator=(const Managed<Other>& arg)
    {
        certify(static_cast<Type*>(arg.the_handle->var));
        arg.the_handle->cnt++;
        if(!--the_handle->cnt)
        {
            auto var = the_handle->var;
            var->~Type();
            release<Handle>(the_handle);
        }
        the_handle = reinterpret_cast<Handle*>(arg.the_handle);
        return *this;
    }
    friend bool operator==(const Nil&, const Managed& arg)
    {
        return (nil_handle() == arg.the_handle);
    }
    friend bool operator!=(const Nil&, const Managed& arg)
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
    bool operator==(const Managed& arg) const
    {
        return (the_handle == arg.the_handle);
    }
    bool operator!=(const Managed& arg) const
    {
        return (the_handle != arg.the_handle);
    }
    template<class Other>
    bool operator==(const Managed<Other>& arg) const
    {
        return (the_handle == reinterpret_cast<Handle*>(arg.the_handle));
    }
    template<class Other>
    bool operator!=(const Managed<Other>& arg) const
    {
        return (the_handle != reinterpret_cast<Handle*>(arg.the_handle));
    }
    Type* operator->()
    {
        certify(the_handle != nil_handle());
        return the_handle->var;
    }
    operator Type&()
    {
        certify(the_handle != nil_handle());
        return *the_handle->var;
    }
    template<class Other>
    bool type_of() const
    {
        certify(the_handle != nil_handle());
        return dynamic_cast<Other*>(the_handle->var);
    }
    template<class Other>
    Other& down_cast() const
    {
        certify(the_handle != nil_handle());
        return dynamic_cast<Other&>(*the_handle->var);
    }
private:
    struct Handle
    {
        uint16 cnt;
        Type var[1];
    };
private:
    static Handle* nil_handle()
    {
        static uint16 nil[1] = { 1 };
        return reinterpret_cast<Handle*>(&nil);
    }
private:
    Handle* the_handle;
};

namespace manage
{
    typedef Managed<int8> int8;
    typedef Managed<int16> int16;
    typedef Managed<int32> int32;
    typedef Managed<int64> int64;

    typedef Managed<uint8> uint8;
    typedef Managed<uint16> uint16;
    typedef Managed<uint32> uint32;
    typedef Managed<uint64> uint64;

    typedef Managed<float32> float32;
    typedef Managed<float64> float64;
    typedef Managed<float128> float128;
}
