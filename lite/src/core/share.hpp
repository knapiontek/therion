
template<class Type>
class Shared
{
    template<class Other>
    friend class Shared;
public:
    Shared()
    {
        the_var = 0;
    }
    Shared(const Nil&)
    {
        the_var = 0;
    }
    Shared(const Type& var)
    {
        the_var = const_cast<Type*>(&var);
    }
    Shared(const Shared& arg)
    {
        the_var = arg.the_var;
    }
    template<class Other>
    Shared(const Shared<Other>& arg)
    {
        the_var = static_cast<Type*>(arg.the_var);
    }
    Shared& operator=(const Nil&)
    {
        the_var = 0;
        return *this;
    }
    Shared& operator=(const Type& var)
    {
        the_var = const_cast<Type*>(&var);
        return *this;
    }
    Shared& operator=(const Shared& arg)
    {
        the_var = arg.the_var;
        return *this;
    }
    template<class Other>
    Shared& operator=(const Shared<Other>& arg)
    {
        the_var = static_cast<Type*>(arg.the_var);
        return *this;
    }
    friend bool operator==(const Nil&, const Shared& arg)
    {
        return (0 == arg.the_var);
    }
    friend bool operator!=(const Nil&, const Shared& arg)
    {
        return (0 != arg.the_var);
    }
    bool operator==(const Nil&) const
    {
        return (0 == the_var);
    }
    bool operator!=(const Nil&) const
    {
        return (0 != the_var);
    }
    bool operator==(const Shared& arg) const
    {
        return (the_var == arg.the_var);
    }
    bool operator!=(const Shared& arg) const
    {
        return (the_var != arg.the_var);
    }
    template<class Other>
    bool operator==(const Shared<Other>& arg) const
    {
        return (the_var == static_cast<Type*>(arg.the_var));
    }
    template<class Other>
    bool operator!=(const Shared<Other>& arg) const
    {
        return (the_var != static_cast<Type*>(arg.the_var));
    }
    Type* operator->()
    {
        certify(the_var);
        return the_var;
    }
    operator Type&()
    {
        certify(the_var);
        return *the_var;
    }
    template<class Other>
    bool type_of() const
    {
        return dynamic_cast<Other*>(the_var);
    }
    template<class Other>
    Other& down_cast() const
    {
        return dynamic_cast<Other&>(*the_var);
    }
private:
    Type* the_var;
};

namespace share
{
    typedef Shared<int8> int8;
    typedef Shared<int16> int16;
    typedef Shared<int32> int32;
    typedef Shared<int64> int64;

    typedef Shared<uint8> uint8;
    typedef Shared<uint16> uint16;
    typedef Shared<uint32> uint32;
    typedef Shared<uint64> uint64;

    typedef Shared<float32> float32;
    typedef Shared<float64> float64;
    typedef Shared<float128> float128;
}
