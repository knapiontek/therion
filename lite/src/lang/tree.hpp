
// util

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

class SyntaxException
{

};

// basic types

enum struct Operator
{
    MUL, DIV, ADD, SUB,
    SHL, SHR, EQ, NE, LT, GT, LE, GE,
    AND, OR, XOR, MOD, NOT
};

enum struct Type
{
    INT8, INT16, INT32, INT64,
    FLOAT32, FLOAT64
};

struct Final
{
    typedef core::Shared<Final> share;
    Token value;
    Type type;
};

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
    Final::share final;
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
    ExtendedVar() : var_list(0x8) {}

    SimpleVar::share var;
    core::List<Var::share> var_list;
};

// tree

class Tree
{
public:
    Tree()
    {
        the_context.size(1);
        the_context[0] = the_var;
    }
    core::List<Var::share>& var_list()
    {
        return the_var.var_list;
    }
    void var(Token& ind, Token& id, Expression& exp)
    {
        auto& var = pager.acquire<SimpleVar>();
        var.id = id;
        var.expression = exp;

        auto size = ind.size();
        auto shift = size / 4;
        if(size % 4 || (shift > the_context.size() - 1))
        {
            throw SyntaxException();
        }

        auto parent = the_context[shift];
        if(parent.type_of<SimpleVar>())
        {
            auto& extended = pager.acquire<ExtendedVar>();
            extended.var = parent;
            extended.var_list.append(var);
            the_context[shift] = extended;

            auto& grand_parent = the_context[shift - 1].down_cast<ExtendedVar>();
            auto tail = grand_parent.var_list.tail();
            if(tail.prev())
                tail.value() = extended;
        }
        else if(parent.type_of<ExtendedVar>())
        {
            auto& extended = parent.down_cast<ExtendedVar>();
            extended.var_list.append(var);
        }

        the_context.size(shift + 2);
        the_context[shift + 1] = var;
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
    core::Pager pager;
    ExtendedVar the_var;
    core::Seq<Var::share> the_context;
};
