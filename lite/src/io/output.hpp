
class Output
{
public:
    virtual ~Output()
    {

    }
    virtual void output(core::uint8* /*data*/, core::uint64 /*size*/)
    {
        core::assert(false);
    }
    virtual void flush()
    {
        core::assert(false);
    }
};

class Encode
{
public:
    Encode(Output& output) : the_output(output)
    {

    }
    void write_st(core::String& arg)
    {
        write(core::uint16(arg.size()));
        write(arg);
    }
    template<typename Type>
    void write(core::Seq<Type>& arg)
    {
        the_output.output(arg.content(), sizeof(Type) * arg.size());
    }
    void write(core::uint8* data, core::uint64 size)
    {
        the_output.output(data, size);
    }
    void write(core::String& arg)
    {
        the_output.output(arg.data(), arg.size());
    }
    void write(core::int8 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::int16 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::int32 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::int64 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::uint8 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::uint16 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::uint32 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::uint64 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::float32 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::float64 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
    void write(core::float128 arg)
    {
        the_output.output((core::uint8*)&arg, sizeof(arg));
    }
private:
    Output& the_output;
};
