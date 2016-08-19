
typedef core::String Token;

class Type
{
public:
    enum
    {
        boolean,
        int8, int16, int32, int64,
        float32, float64, float128
    };
};

class Var
{

};

template<typename Type>
struct Ref
{
    Type* type;
    operator Type&()
    {
        return *type;
    }
};

template<typename Type>
Ref<Type> ref(Type& type)
{
    Ref<Type> ref {&type};
    return ref;
}

Var the_var;
Type the_type;
class SyntaxException {};

class Tree
{
public:
    Ref<Var> var(Token&, Type&)
    {
        return ref(the_var);
    }
    Ref<Type> type(Token&)
    {
        return ref(the_type);
    }
    Ref<Type> type(int)
    {
        return ref(the_type);
    }
    void syntax_error()
    {
        throw SyntaxException();
    }
};
