
typedef core::String Token;

class Type
{
public:
    enum
    {
        boolean,
        int8, int16, int32, int64,
        uint8, uint16, uint32, uint64,
        float32, float64, float128
    };
};

class Var
{

};

class Tree
{
public:
    Var* var(Token*, Type*)
    {
        return 0;
    }
    Type* type(Token*)
    {
        return 0;
    }
    Type* type(int)
    {
        return 0;
    }
    void syntax_error()
    {

    }
};
