
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
    virtual core::String& get_id() = 0;
    virtual ~Var() {}
};

// location

struct IdLocation : Location
{
    Token id;
    core::int64 field_pos;
    Var::share context_var;
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
    core::String& get_id() override { return id; }

    Token id;
};

struct AssignVar : Var
{
    core::String& get_id() override { return id; }

    Token id;
    Expression::share exp;
};

struct ClazzVar : Var
{
    ClazzVar() : field_list(0x8) {}
    core::String& get_id() override { return signature->get_id(); }

    Var::share signature;
    core::List<Var::share> field_list;
};

// tree

class Tree
{
public:
    Tree()
    {
        the_context.append(the_var);
    }
    ClazzVar& var()
    {
        return the_var;
    }
    void var(Token& id, Expression& exp)
    {
        auto& var = the_pager.acquire<AssignVar>();
        var.id = id;
        var.exp = exp;

        auto last = the_context[the_context.size() - 1];
        auto& clazz_var = last.down_cast<ClazzVar>();
        clazz_var.field_list.append(var);
    }
    void var(Token& id)
    {
        auto& var = the_pager.acquire<IdVar>();
        var.id = id;

        auto last = the_context[the_context.size() - 1];
        auto& clazz_var = last.down_cast<ClazzVar>();
        clazz_var.field_list.append(var);
    }
    void ind(Token& ind)
    {
        auto size = ind.size();
        auto shift = size / 4;
        if(size % 4 || (shift > the_context.size() - 1))
        {
            throw SyntaxException();
        }

        auto last = the_context[shift];
        if(!last.type_of<ClazzVar>())
        {
            auto& clazz_var = the_pager.acquire<ClazzVar>();
            clazz_var.signature = last;
            the_context[shift] = clazz_var;

            auto& grand_context = the_context[shift - 1].down_cast<ClazzVar>();
            auto tail = grand_context.field_list.tail();
            if(tail.prev())
                tail.value() = clazz_var;
        }

        the_context.size(shift + 1);
    }
    Ret<Expression> exp(Expression& exp1, BinaryOp op, Expression& exp2)
    {
        auto& exp = the_pager.acquire<NestExpression>();
        exp.exp1 = exp1;
        exp.op = op;
        exp.exp2 = exp2;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp1, BinaryOp op, Location& loc)
    {
        auto& exp = the_pager.acquire<NestLocationExpression>();
        exp.exp = exp1;
        exp.op = op;
        exp.loc = loc;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Expression& exp1, BinaryOp op, Final& final)
    {
        auto& exp = the_pager.acquire<NestFinalExpression>();
        exp.exp = exp1;
        exp.op = op;
        exp.final = final;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Location& loc)
    {
        auto& exp = the_pager.acquire<LocationExpression>();
        exp.loc = loc;
        return ret<Expression>(exp);
    }
    Ret<Expression> exp(Final& final)
    {
        auto& exp = the_pager.acquire<FinalExpression>();
        exp.final = final;
        return ret<Expression>(exp);
    }
    Ret<Location> loc(Location& loc1, Token& id, Expression& exp)
    {
        auto& loc = the_pager.acquire<NestFilterLocation>();
        loc.loc = loc1;
        loc.id = id;
        loc.exp = exp;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Location& loc1, Token& id)
    {
        auto& loc = the_pager.acquire<NestIdLocation>();
        loc.loc = loc1;
        loc.id = id;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Token& id, Expression& exp)
    {
        auto& loc = the_pager.acquire<FilterLocation>();
        loc.id = id;
        loc.exp = exp;
        return ret<Location>(loc);
    }
    Ret<Location> loc(Token& id)
    {
        for(auto& context_it : core::reverse(the_context))
        {
            auto& var = context_it.value();
            if(var.type_of<ClazzVar>())
            {
                auto& clazz_var = var.down_cast<ClazzVar>();
                for(auto& field_it : clazz_var.field_list)
                {
                    auto& field = field_it.value();
                    if(id.equal(field->get_id()))
                    {
                        auto& loc = the_pager.acquire<IdLocation>();
                        loc.id = id;
                        loc.field_pos = field_it.position();
                        loc.context_var = context_it.value();
                        return ret<Location>(loc);
                    }
                }
            }
        }
        throw env::Format("Unknown variable: %1") % id % env::exception;
    }
    Ret<Final> final(Token& value, Type type)
    {
        auto& final = the_pager.acquire<Final>();
        final.value = value;
        final.type = type;
        return ret<Final>(final);
    }
private:
    core::Pager the_pager;
    ClazzVar the_var;
    core::Seq<Var::share> the_context;
};
