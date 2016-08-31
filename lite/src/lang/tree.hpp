
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
inline Ref<Class> ref(Class& clazz)
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

struct Writable
{

};

struct Loc : Writable
{

};

struct Exp : Writable
{

};

struct BinaryOpExp : Writable
{

};

struct Var : Writable
{

};

class SyntaxException
{

};

class Tree
{
public:
    Ref<Var> var(Token& id, Exp& exp)
    {
        static Var var;
        return ref(var);
    }
    Ref<Exp> exp(Exp& exp1, Op o, Exp& exp2)
    {
        static Exp exp;
        return ref(exp);
    }
    Ref<Exp> exp(Exp& exp1, Op o, Loc& loc)
    {
        static Exp exp;
        return ref(exp);
    }
    Ref<Exp> exp(Exp& exp1, Op o, Type& type)
    {
        static Exp exp;
        return ref(exp);
    }
    Ref<Exp> exp(Loc& loc)
    {
        static Exp exp;
        return ref(exp);
    }
    Ref<Exp> exp(Type& type)
    {
        static Exp exp;
        return ref(exp);
    }
    Ref<Loc> loc(Loc& loc1, Token& id, Exp& exp)
    {
        static Loc loc;
        return ref(loc);
    }
    Ref<Loc> loc(Loc& loc1, Token& id)
    {
        static Loc loc;
        return ref(loc);
    }
    Ref<Loc> loc(Token& id, Exp& exp)
    {
        static Loc loc;
        return ref(loc);
    }
    Ref<Loc> loc(Token& id)
    {
        static Loc loc;
        return ref(loc);
    }
    void syntax_error()
    {
        throw SyntaxException();
    }
};
