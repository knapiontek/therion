
typedef core::String Token;

class Type
{
public:
    enum Enum
    {
        BOOL,
        INT8, INT16, INT32, INT64,
        UINT8, UINT16, UINT32, UINT64,
        FLOAT32, FLOAT64, FLOAT128
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
