
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
    BOOL,
    INT8, INT16, INT32, INT64,
    FLOAT32, FLOAT64, FLOAT128
};

enum class Op
{
    MUL, DIV, ADD, SUB,
    SHL, SHR, EQ, NE, LT, GT, LE, GE,
    AND, OR, XOR, MOD, NOT
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
