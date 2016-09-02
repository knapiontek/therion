
// util

class SyntaxException
{

};

typedef core::String Token;

template<class Type>
struct Ret
{
    Type* type;
    operator Type&()
    {
        return *type;
    }
};

template<class Type>
inline Ret<Type> ret(Type& type)
{
    Ret<Type> ret {&type};
    return ret;
}

// interfaces

struct Location
{
    typedef core::Shared<Location> share;
    virtual ~Location() {}
};

struct Expression
{
    typedef core::Shared<Expression> share;
    virtual ~Expression() {}
};

struct Var
{
    typedef core::Shared<Var> share;
    virtual ~Var() {}
};

// operator

enum struct Operator
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

struct Final
{
    Token value;
    Type type;
};

// location

struct IdLocation : Location
{
    Token id;
};

struct SeqLocation : Location
{
    Token id;
    Expression::share filter;
};

struct NestedLocation : Location
{
    Location::share nest;
    Token id;
};

struct NestedSeqLocation : Location
{
    Location::share nest;
    Token id;
    Expression::share filter;
};

// expression

struct FinalExpression : Expression
{
    Final final;
};

struct LocationExpression : Expression
{
    Location::share loc;
};

struct FinalNestedExpression : Expression
{
    Expression::share nest;
    Operator op;
    Final final;
};

struct LocationNestedExpression : Expression
{
    Expression::share nest;
    Operator op;
    Location::share loc;
};

struct NestedExpression : Expression
{
    Expression::share nest1;
    Operator op;
    Expression::share nest2;
};

struct SimpleVar : Var
{
    Token id;
    Expression::share expression;
};

struct ExtendedVar : Var
{
    Token id;
    Expression::share expression;
    core::List<Var::share> var_list;
};

class Tree
{
public:
    Tree() : the_var_list(0x40)
    {

    }
    core::List<Var::share> var_list()
    {
        return the_var_list;
    }
    Ret<Var> var(Token& id, Expression& exp)
    {
        auto& var = pager.acquire<SimpleVar>();
        var.id = id;
        var.expression = exp;
        return ret<Var>(var);
    }
    Ret<Expression> exp(Expression& exp1, Operator op, Expression& exp2)
    {
        auto& exp = pager.acquire<NestedExpression>();
        exp.nest1 = exp1;
        exp.op = op;
        exp.nest2 = exp2;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp1, Operator op, Location& loc)
    {
        auto& exp = pager.acquire<LocationNestedExpression>();
        exp.nest = exp1;
        exp.op = op;
        exp.loc = loc;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp1, Operator op, Final& final)
    {
        auto& exp = pager.acquire<FinalNestedExpression>();
        exp.nest = exp1;
        exp.op = op;
        exp.final = final;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Location& loc)
    {
        auto& exp = pager.acquire<LocationExpression>();
        exp.loc = loc;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Final& final)
    {
        auto& exp = pager.acquire<FinalExpression>();
        exp.final = final;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp)
    {
        return ret<Expression>(exp);
    }
    Ret<Location> loc(Location& loc1, Token& id, Expression& exp)
    {
        auto& loc = pager.acquire<NestedSeqLocation>();
        loc.nest = loc1;
        loc.id = id;
        loc.filter = exp;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Location& loc1, Token& id)
    {
        auto& loc = pager.acquire<NestedLocation>();
        loc.nest = loc1;
        loc.id = id;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Token& id, Expression& exp)
    {
        auto& loc = pager.acquire<SeqLocation>();
        loc.id = id;
        loc.filter = exp;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Token& id)
    {
        auto& loc = pager.acquire<IdLocation>();
        loc.id = id;
        return ret<Location>(loc);
    }
    Ret<Final> final(Token& value, Type type)
    {
        auto& final = pager.acquire<Final>();
        final.value = value;
        final.type = type;
        return ret<Final>(final);
    }
private:
    core::List<Var::share> the_var_list;
    core::Pager pager;
};
