
class Input
{
public:
    virtual ~Input()
    {

    }
    virtual void input(core::uint8* data, core::int64 size) = 0;
    virtual void skip(core::int64 size) = 0;
    virtual core::int64 available() = 0;
};

class Decode
{
public:
    Decode(Input& input) : the_input(input)
    {

    }
    void read_st(core::String& arg)
    {
        core::uint16 size;
        read(size);
        arg.size(size);
        read(arg);
    }
    template<typename Type>
    void read(core::Seq<Type>& arg)
    {
        the_input.input(arg.content(), sizeof(Type) * arg.size());
    }
    void read(core::uint8* data, core::int64 size)
    {
        the_input.input(data, size);
    }
    void read(core::String& arg)
    {
        the_input.input(arg.data(), arg.size());
    }
    void read(core::int8& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::int16& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::int32& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::int64& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::uint8& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::uint16& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::uint32& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::uint64& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::float32& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::float64& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void read(core::float128& arg)
    {
        the_input.input((core::uint8*)&arg, sizeof(arg));
    }
    void skip(core::int64 size)
    {
        the_input.skip(size);
    }
    core::int64 available()
    {
        return the_input.available();
    }
private:
    Input& the_input;
};
