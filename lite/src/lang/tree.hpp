
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

struct WriterClass
{
    virtual void execute(struct Loc&) = 0;
    virtual void execute(struct Exp&) = 0;
    virtual void execute(struct BinaryOpExp&) = 0;
    virtual void execute(struct Var&) = 0;
};

struct Writable
{
    virtual void execute(WriterClass&) = 0;
};

struct Loc : Writable
{
    void execute(WriterClass& writer) override
    {
        writer.execute(*this);
    }
};

struct Exp : Writable
{
    void execute(WriterClass& writer) override
    {
        writer.execute(*this);
    }
};

struct BinaryOpExp : Exp
{
    void execute(WriterClass& writer) override
    {
        writer.execute(*this);
    }
};

struct Var : Writable
{
    void execute(WriterClass& writer) override
    {
        writer.execute(*this);
    }
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
