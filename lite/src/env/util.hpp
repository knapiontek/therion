
class Return
{
public:
    enum
    {
        log_fatal = 0x1,
        log_err = 0x2,
        fail = 0x4,
        throw_alloc_exception = 0x8,
        signal = 0x10
    };
public:
    static core::int64& code()
    {
        static core::int64 code = 0;
        return code;
    }
};

core::String cs_F = "F: ";
core::String cs_E = "E: ";
core::String cs_W = "W: ";
core::String cs_I = "I: ";
core::String cs_D = "D: ";

class Log : public core::FormatClass<Log>
{
public:
    static bool& verbose()
    {
        static bool verbose = false;
        return verbose;
    }
public:
    Log()
    {
        the_size = 0;
    }
    ~Log()
    {
    }
    Log& fatal(core::String& format)
    {
        Return::code() |= Return::log_fatal;
        core::FormatClass<Log>::format(format);
        the_type = cs_F;
        return *this;
    }
    Log& err(core::String& format)
    {
        Return::code() |= Return::log_err;
        core::FormatClass<Log>::format(format);
        the_type = cs_E;
        return *this;
    }
    Log& warn(core::String& format)
    {
        core::FormatClass<Log>::format(format);
        the_type = cs_W;
        return *this;
    }
    Log& info(core::String& format)
    {
        core::FormatClass<Log>::format(format);
        the_type = cs_I;
        return *this;
    }
    Log& debug(core::String& format)
    {
        core::FormatClass<Log>::format(verbose() ? format : core::nil);
        the_type = cs_D;
        return *this;
    }
    void end()
    {
        if(verbose() || the_type != cs_D)
        {
            if(Device::device().used())
                Device::device().writeln();
            Device::device().write(the_type);
            auto& arg = core::FormatClass<Log>::end();
            Device::device().write(arg);
            Device::device().flush();
        }
    }
    void endln()
    {
        core::assert(false);
    }
private:
    core::String the_type;
    core::int64 the_size;
};

class Throw : public core::FormatClass<Throw>
{
public:
    static void raise(core::String& message)
    {
        Return::code() |= Return::fail;
        Exception exception;
        exception.message() = message;
        throw exception;
    }
public:
    Throw(core::String& format) : core::FormatClass<Throw>(format)
    {

    }
    void end()
    {
        Return::code() |= Return::fail;
        Exception exception;
        exception.message() = core::FormatClass<Throw>::end();
        throw exception;
    }
    void endln()
    {
        core::assert(false);
    }
};

class Signal
{
public:
    static void signal()
    {
        ::signal(SIGABRT, handler);
        ::signal(SIGFPE, handler);
        ::signal(SIGILL, handler);
        ::signal(SIGINT, handler);
        ::signal(SIGSEGV, handler);
        ::signal(SIGTERM, handler);
    }
private:
    class RecurrenceBarrier
    {
    public:
        ~RecurrenceBarrier()
        {
            __sync_fetch_and_sub(cnt(), 1);
        }
        bool operator++(int)
        {
            return __sync_fetch_and_add(cnt(), 1);
        }
    private:
        static core::int64* cnt()
        {
            static __thread core::int64 cnt = 0;
            return &cnt;
        }
    };
private:
    static void handler(int id)
    {
        RecurrenceBarrier cnt;
        if(cnt++) return;

        Return::code() |= Return::signal;
        Exception exception;
        switch(id)
        {
        case SIGABRT:
            exception.message() = "captured signal: Abort";
            break;
        case SIGFPE:
            exception.message() = "captured signal: Floating-Point Exception";
            break;
        case SIGILL:
            exception.message() = "captured signal: Illegal Instruction";
            break;
        case SIGINT:
            exception.message() = "captured signal: Interrupt";
            break;
        case SIGSEGV:
            exception.message() = "captured signal: Segmentation Violation";
            break;
        case SIGTERM:
            exception.message() = "captured signal: Terminate";
            break;
        default:
            core::assert(false);
        }
        throw exception;
    }
};

class Handler : public core::Handler
{
public:
    static void handler()
    {
        static Handler handler;
        core::Handler::handler() = &handler;
    }
public:
    void throw_alloc_exception()
    {
        // ::malloc not allowed in here!
        Return::code() |= Return::throw_alloc_exception;
        Exception exception;
        exception.message() = "allocation exception";
        throw exception;
    }
    void call_assert(const char* file_name, int line_no, const char* content)
    {
        auto message = core::Format("\nassert:\n\t$1 [$2:$3]\nbacktrace:")
            .arg(content)
            .arg(file_name)
            .arg(line_no)
            .end();
        auto pos = message.size();

        Runtime::Iterator it = Runtime::backtrace();
        while(it.next())
        {
            auto& frame = it.value();
            auto st_frame = core::Format("\n\t$1 [$2:$3]")
                .arg(frame.function_name)
                .arg(frame.file_name)
                .arg(frame.line)
                .end();
            message.copy_in(pos, st_frame);
        }

        message.size(pos);
        Device::device().write(message);
    }
};

inline core::Seq<core::String>& arg_seq()
{
    static core::Seq<core::String> arg_seq;
    return arg_seq;
}

inline void init(int argc, char* argv[])
{
    Signal::signal();
    Handler::handler();

    arg_seq().size(argc);
    for(auto i = 0; i < argc; i++)
        arg_seq().at(i) = argv[i];
}

inline core::int64 final()
{
    if(Device::device().used())
        Device::device().writeln();
    return Return::code();
}
