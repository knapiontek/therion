
// util

typedef core::String Token;

template<class Class>
struct Ret
{
    Class* clazz;
    operator Class&()
    {
        return *clazz;
    }
};

template<class Class>
inline Ret<Class> ret(Class& clazz)
{
    Ret<Class> ret {&clazz};
    return ret;
}

struct WriterClass
{

};

// op

enum struct Op
{
    MUL, DIV, ADD, SUB,
    SHL, SHR, EQ, NE, LT, GT, LE, GE,
    AND, OR, XOR, MOD, NOT
};

// type

enum struct Type
{
    BOOL,
    INT8, INT16, INT32, INT64,
    FLOAT32, FLOAT64, FLOAT128
};

// loc

struct Loc
{
    typedef core::Shared<Loc> share;
    Token id;
};

struct LocExp : Loc
{
    core::Shared<struct Exp> exp;
};

struct LocNested : Loc
{
    Loc::share nest;
};

struct LocExpNested : LocExp
{
    Loc::share nest;
};

// exp

struct Exp
{
    typedef core::Shared<Exp> share;
};

struct ExpType : Exp
{
    Type type;
};

struct ExpLoc : Exp
{
    Loc::share loc;
};

struct ExpOpType : Exp
{
    Exp::share exp1;
    Op op;
    Type type;
};

struct ExpOpLoc : Exp
{
    Exp::share exp1;
    Op op;
    Loc::share loc;
};

struct ExpOpExp : Exp
{
    Exp::share exp1;
    Op op;
    Exp::share exp2;
};

struct Var
{
    typedef core::Shared<Var> share;
    Token id;
    Exp::share exp;
};

class SyntaxException
{

};

class Tree
{
public:
    Tree() : var_list(0x40)
    {

    }
    Ret<Var> var(Token& id, Exp& exp)
    {
        auto& var = pager.acquire<Var>();
        var.id = id;
        var.exp = exp;
        return ret<Var>(var);
    }
    Ret<Exp> exp(Exp& exp1, Op op, Exp& exp2)
    {
        auto& exp = pager.acquire<ExpOpExp>();
        exp.exp1 = exp1;
        exp.op = op;
        exp.exp2 = exp2;
        return ret<Exp>(exp);
    }
    Ret<Exp> exp(Exp& exp1, Op op, Loc& loc)
    {
        auto& exp = pager.acquire<ExpOpLoc>();
        exp.exp1 = exp1;
        exp.op = op;
        exp.loc = loc;
        return ret<Exp>(exp);
    }
    Ret<Exp> exp(Exp& exp1, Op op, Type& type)
    {
        auto& exp = pager.acquire<ExpOpType>();
        exp.exp1 = exp1;
        exp.op = op;
        exp.type = type;
        return ret<Exp>(exp);
    }
    Ret<Exp> exp(Loc& loc)
    {
        auto& exp = pager.acquire<ExpLoc>();
        exp.loc = loc;
        return ret<Exp>(exp);
    }
    Ret<Exp> exp(Type& type)
    {
        auto& exp = pager.acquire<ExpType>();
        exp.type = type;
        return ret<Exp>(exp);
    }
    Ret<Exp> exp(Exp& exp)
    {
        return ret<Exp>(exp);
    }
    Ret<Loc> loc(Loc& loc1, Token& id, Exp& exp)
    {
        auto& loc = pager.acquire<LocExpNested>();
        loc.nest = loc1;
        loc.id = id;
        loc.exp = exp;
        return ret<Loc>(loc);
    }
    Ret<Loc> loc(Loc& loc1, Token& id)
    {
        auto& loc = pager.acquire<LocNested>();
        loc.nest = loc1;
        loc.id = id;
        return ret<Loc>(loc);
    }
    Ret<Loc> loc(Token& id, Exp& exp)
    {
        auto& loc = pager.acquire<LocExp>();
        loc.id = id;
        loc.exp = exp;
        return ret<Loc>(loc);
    }
    Ret<Loc> loc(Token& id)
    {
        auto& loc = pager.acquire<Loc>();
        loc.id = id;
        return ret<Loc>(loc);
    }
public:
    core::Pager pager;
    core::List<Var::share> var_list;
};
