
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

struct TypeExpression : Expression
{
    Type type;
};

struct LocationExpression : Expression
{
    Location::share loc;
};

struct TypeNestedExpression : Expression
{
    Expression::share nest;
    Operator op;
    Type type;
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
    Ret<Expression> exp(Expression& exp1, Operator op, Type& type)
    {
        auto& exp = pager.acquire<TypeNestedExpression>();
        exp.nest = exp1;
        exp.op = op;
        exp.type = type;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Location& loc)
    {
        auto& exp = pager.acquire<LocationExpression>();
        exp.loc = loc;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Type& type)
    {
        auto& exp = pager.acquire<TypeExpression>();
        exp.type = type;
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
private:
    core::List<Var::share> the_var_list;
    core::Pager pager;
};
