
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

Var the_var;
Exp the_exp;
Loc the_loc;
class SyntaxException {};

class Tree
{
public:
    Ref<Var> var(Token&, Exp&)
    {
        return ref(the_var);
    }
    Ref<Exp> exp(Exp&, Type&)
    {
        return ref(the_exp);
    }
    Ref<Exp> exp(Type&)
    {
        return ref(the_exp);
    }
    void syntax_error()
    {
        throw SyntaxException();
    }
};
