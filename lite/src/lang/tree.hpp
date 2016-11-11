
// util

typedef core::String Token;

template<class Type>
struct Ret
{
    Type* ret;
    operator Type&()
    {
        return *ret;
    }
};

template<class Type>
inline Ret<Type> ret(Type& var)
{
    Ret<Type> ret {&var};
    return ret;
}

class SyntaxException
{

};

// basic types

enum struct UnaryOp
{
    NOT, NEG, INC, DEC
};

enum struct BinaryOp
{
    MUL, DIV, ADD, SUB,
    SHL, SHR,
    EQ, NE, LT, GT, LE, GE,
    AND, OR, XOR, MOD
};

inline core::String& binary_op_name(BinaryOp op)
{
    static core::String names[] = { "*", "/", "+", "-",
                                    "<<", ">>",
                                    "=", "<>", "<", ">", "<=", ">=",
                                    "&", "|", "^", "%" };
    core::certify(op < BinaryOp(sizeof(names)/sizeof(*names)));
    return names[core::int64(op)];
}

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

struct FilterLocation : Location
{
    Token id;
    Expression::share exp;
};

struct NestIdLocation : Location
{
    Location::share loc;
    Token id;
};

struct NestFilterLocation : Location
{
    Location::share loc;
    Token id;
    Expression::share exp;
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

struct NestFinalExpression : Expression
{
    Expression::share exp;
    BinaryOp op;
    Final final;
};

struct NestLocationExpression : Expression
{
    Expression::share exp;
    BinaryOp op;
    Location::share loc;
};

struct NestExpression : Expression
{
    Expression::share exp1;
    BinaryOp op;
    Expression::share exp2;
};

struct IdVar : Var
{
    Token id;
};

struct AssignVar : Var
{
    Token id;
    Expression::share exp;
};

struct CompositeVar : Var
{
    CompositeVar() : var_list(0x8) {}

    Var::share signature_var;
    core::List<Var::share> var_list;
};

// tree

class Tree
{
public:
    Tree()
    {
        auto& signature_var = pager.acquire<IdVar>();
        signature_var.id = "main";
        the_var.signature_var = signature_var;
        the_context.append(the_var);
    }
    CompositeVar& var()
    {
        return the_var;
    }
    void var(Token& indent, Token& id, Expression& exp)
    {
        auto& var = pager.acquire<AssignVar>();
        var.id = id;
        var.exp = exp;
        update_context(indent, var);
    }
    void var(Token& indent, Token& id)
    {
        auto& var = pager.acquire<IdVar>();
        var.id = id;
        update_context(indent, var);
    }
    Ret<Expression> exp(Expression& exp1, BinaryOp op, Expression& exp2)
    {
        auto& exp = pager.acquire<NestExpression>();
        exp.exp1 = exp1;
        exp.op = op;
        exp.exp2 = exp2;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp1, BinaryOp op, Location& loc)
    {
        auto& exp = pager.acquire<NestLocationExpression>();
        exp.exp = exp1;
        exp.op = op;
        exp.loc = loc;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp1, BinaryOp op, Final& final)
    {
        auto& exp = pager.acquire<NestFinalExpression>();
        exp.exp = exp1;
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
    Ret<Location> loc(Location& loc1, Token& id, Expression& exp)
    {
        auto& loc = pager.acquire<NestFilterLocation>();
        loc.loc = loc1;
        loc.id = id;
        loc.exp = exp;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Location& loc1, Token& id)
    {
        auto& loc = pager.acquire<NestIdLocation>();
        loc.loc = loc1;
        loc.id = id;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Token& id, Expression& exp)
    {
        auto& loc = pager.acquire<FilterLocation>();
        loc.id = id;
        loc.exp = exp;
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
    void update_context(Token& indent, Var& var)
    {
        auto size = indent.size();
        auto shift = size / 4;
        if(size % 4 || (shift > the_context.size() - 1))
        {
            throw SyntaxException();
        }

        auto parent = the_context[shift];
        if(parent.type_of<CompositeVar>())
        {
            auto& composite = parent.down_cast<CompositeVar>();
            composite.var_list.append(var);
        }
        else
        {
            auto& composite = pager.acquire<CompositeVar>();
            composite.signature_var = parent;
            composite.var_list.append(var);
            the_context[shift] = composite;

            auto& grand_parent = the_context[shift - 1].down_cast<CompositeVar>();
            auto tail = grand_parent.var_list.tail();
            if(tail.prev())
                tail.value() = composite;
        }

        the_context.size(shift + 2);
        the_context[shift + 1] = var;
    }
private:
    core::Pager pager;
    CompositeVar the_var;
    core::Seq<Var::share> the_context;
};
