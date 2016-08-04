
class Con : public core::FormatClass<Con>
{
public:
    static void write(core::String& arg)
    {
        Device::device().write(arg);
    }
    static void writeln(core::String& arg)
    {
        Device::device().writeln(arg);
    }
    static void exception(Exception& exception)
    {
        core::uint32 pos = 0;
        core::String st_exception;

        // header
        st_exception.copy_in(pos, "\nexception:");
        st_exception.copy_in(pos, "\n\t");
        st_exception.copy_in(pos, exception.message());

        // backtrace
        st_exception.copy_in(pos, "\nbacktrace:");
        Runtime::Iterator frame_it = exception.runtime();
        while(frame_it.next())
        {
            Runtime::Frame& frame = frame_it.value();
            core::String st_frame = core::Format(cs_frame)
                .arg(frame.function_name)
                .arg(frame.file_name)
                .arg(frame.line)
                .end();
            st_exception.copy_in(pos, st_frame);
        }

        // write
        st_exception.size(pos);
        Device::device().write(st_exception);
    }
public:
    Con(core::String& format) : core::FormatClass<Con>(format)
    {

    }
    void end()
    {
        Device::device().write(core::FormatClass<Con>::end());
    }
    void endln()
    {
        Device::device().writeln(core::FormatClass<Con>::end());
    }
};
