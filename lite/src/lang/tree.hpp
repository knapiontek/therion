
typedef core::String Token;

template<class Class>
struct Ref
{
    Class* clazz;
    operator Class&()
    {
        return *clazz;
    }
};

template<class Class>
Ref<Class> ref(Class& clazz)
{
    Ref<Class> ref {&clazz};
    return ref;
}

enum class Type
{
    _bool,
    int8, int16, int32, int64,
    float32, float64, float128
};

enum class Op
{
    mul, div, add, sub, shl, shr, eq, ne, lt, gt, le, ge, _and, _or, _xor, mod, _not
};

class Loc
{

};

class Exp
{

};

class Var
{

};

class SyntaxException
{

};

class Tree
{
public:
    Ref<Var> var(Token&, Exp&)
    {
        static Var the_var;
        return ref(the_var);
    }
    Ref<Exp> exp(Exp&, Type&)
    {
        static Exp the_exp;
        return ref(the_exp);
    }
    Ref<Exp> exp(Type&)
    {
        static Exp the_exp;
        return ref(the_exp);
    }
    void syntax_error()
    {
        throw SyntaxException();
    }
};
