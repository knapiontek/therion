
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

struct ClazzVar;

struct Location
{
    typedef core::Shared<Location> share;
    virtual ~Location() {}

    core::int64 field_pos;
    core::Shared<ClazzVar> context_var;
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
    virtual core::String& get_unique_id() = 0;
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
    core::String& get_id() override { return id; }
    core::String& get_unique_id() override { return unique_id; }

    Token id;
    core::String unique_id;
};

struct AssignVar : Var
{
    core::String& get_id() override { return id; }
    core::String& get_unique_id() override { return unique_id; }

    Token id;
    core::String unique_id;
    Expression::share exp;
};

struct ClazzVar : Var
{
    typedef core::Shared<ClazzVar> share;

    ClazzVar() : field_var_list(0x8) {}
    core::String& get_id() override { return decl_var->get_id(); }
    core::String& get_unique_id() override { return decl_var->get_unique_id(); }

    Var::share decl_var;
    core::List<Var::share> field_var_list;
};

// tree

class Tree
{
public:
    Tree(core::String scope_id)
    {
        auto& scope_var = the_pager.acquire<IdVar>();
        scope_var.id = scope_id;
        scope_var.unique_id = scope_id;
        the_main_var.decl_var = scope_var;
        the_context_seq.append(the_main_var);
    }
    ClazzVar& main_var()
    {
        return the_main_var;
    }
    void var(Token& id, Expression& exp)
    {
        auto& var = the_pager.acquire<AssignVar>();
        var.id = id;
        var.unique_id = unique_id(id);
        var.exp = exp;

        auto last_var = the_context_seq.last();
        last_var->field_var_list.append(var);
    }
    void var(Token& id)
    {
        auto& var = the_pager.acquire<IdVar>();
        var.id = id;
        var.unique_id = unique_id(id);

        auto last_var = the_context_seq.last();
        last_var->field_var_list.append(var);
    }
    void ind(Token& ind)
    {
        auto size = ind.size();
        if(size % 4)
            throw SyntaxException();

        auto shift = 1 + (size / 4);
        auto& last_var = the_context_seq.last();
        auto last_field = last_var->field_var_list.tail();
        auto has_field = last_field.prev();
        if(shift > the_context_seq.size() + has_field)
            throw SyntaxException();

        if((shift == the_context_seq.size() + 1) && has_field)
        {
            auto& clazz_var = the_pager.acquire<ClazzVar>();
            clazz_var.decl_var = last_field.value();
            last_field.value() = clazz_var;
            the_context_seq.append(clazz_var);
        }
        else if(shift < the_context_seq.size())
        {
            the_context_seq.size(shift);
        }
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
        auto& var = loc1.context_var->field_var_list[loc1.field_pos];
        if(var.type_of<ClazzVar>())
        {
            auto& clazz_var = var.down_cast<ClazzVar>();
            for(auto& field_it : clazz_var.field_var_list)
            {
                auto& field_var = field_it.value();
                if(id.equal(field_var->get_id()))
                {
                    auto& loc = the_pager.acquire<NestIdLocation>();
                    loc.loc = loc1;
                    loc.id = id;
                    loc.field_pos = field_it.position();
                    loc.context_var = var;
                    return ret<Location>(loc);
                }
            }
        }
        throw env::Format("Variable: %1 does not contain: %2") % var->get_id() % id % env::exception;
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
        for(auto& context_it : core::reverse(the_context_seq))
        {
            auto& context_var = context_it.value();
            for(auto& field_it : context_var->field_var_list)
            {
                auto& field_var = field_it.value();
                if(id.equal(field_var->get_id()))
                {
                    auto& loc = the_pager.acquire<IdLocation>();
                    loc.id = id;
                    loc.field_pos = field_it.position();
                    loc.context_var = context_var;
                    return ret<Location>(loc);
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
    core::String unique_id(core::String id)
    {
        auto size = id.size();
        for(auto& it : the_context_seq)
        {
            auto& var = it.value();
            size += 1 + var->get_id().size();
        }

        core::int64 pos = 0;
        core::String result;
        result.size(size);
        for(auto& it : the_context_seq)
        {
            auto& var = it.value();
            result.copy_in(pos, var->get_id());
            result.copy_in(pos, "_");
        }
        result.copy_in(pos, id);
        core::certify(pos == result.size());
        return result;
    }
private:
    core::Pager the_pager;
    ClazzVar the_main_var;
    core::Seq<ClazzVar::share> the_context_seq;
};
