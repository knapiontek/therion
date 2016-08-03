
core::String cs_F = cs("F: ");
core::String cs_E = cs("E: ");
core::String cs_W = cs("W: ");
core::String cs_I = cs("I: ");
core::String cs_D = cs("D: ");

core::String cs_abort = cs("captured signal: Abort");
core::String cs_fpe = cs("captured signal: Floating-Point Exception");
core::String cs_ill = cs("captured signal: Illegal Instruction");
core::String cs_interrupt = cs("captured signal: Interrupt");
core::String cs_segv = cs("captured signal: Segmentation Violation");
core::String cs_terminate = cs("captured signal: Terminate");

core::String cs_allocation_exception = cs("allocation exception");
core::String cs_assert = cs("\nassert:\n\t$1 [$2:$3]\nbacktrace:");

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
    static core::int32& code()
    {
        static core::int32 code = 0;
        return code;
    }
};

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
                Device::device().write(cs_line);
            Device::device().write(the_type);
            core::String& arg = core::FormatClass<Log>::end();
            Device::device().write(arg);
            Device::device().flush();
        }
    }
private:
    void endln()
    {

    }
private:
    core::String the_type;
    core::uint32 the_size;
};

class Fail : public core::FormatClass<Fail>
{
public:
    static void fire(core::String& message)
    {
        Return::code() |= Return::fail;
        Exception exception;
        exception.message() = message;
        throw exception;
    }
public:
    Fail(core::String& format) : core::FormatClass<Fail>(format)
    {

    }
    void fire()
    {
        Return::code() |= Return::fail;
        Exception exception;
        exception.message() = core::FormatClass<Fail>::end();
        throw exception;
    }
private:
    void end()
    {

    }
    void endln()
    {

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
        static core::uint16* cnt()
        {
            static __thread core::uint16 cnt = 0;
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
            exception.message() = cs_abort;
            break;
        case SIGFPE:
            exception.message() = cs_fpe;
            break;
        case SIGILL:
            exception.message() = cs_ill;
            break;
        case SIGINT:
            exception.message() = cs_interrupt;
            break;
        case SIGSEGV:
            exception.message() = cs_segv;
            break;
        case SIGTERM:
            exception.message() = cs_terminate;
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
        exception.message() = cs_allocation_exception;
        throw exception;
    }
    void call_assert(const char* file_name, int line_no, const char* content)
    {
        core::String message = core::Format(cs_assert)
            .arg(ascii_len(content))
            .arg(ascii_len(file_name))
            .arg(line_no)
            .end();
        core::uint32 pos = message.size();

        Runtime::Iterator it = Runtime::backtrace();
        while(it.next())
        {
            Runtime::Frame& frame = it.value();
            core::String st_frame = core::Format(cs_frame)
                .arg(ascii_len(frame.function_name))
                .arg(ascii_len(frame.file_name))
                .arg(frame.line)
                .end();
            message.copy_in(pos, st_frame);
        }

        message.size(pos);
        Device::device().write(message);
    }
};

inline void init(int /*argc*/, char* /*argv*/[])
{
    Signal::signal();
    Handler::handler();
}

inline core::int32 final()
{
    if(Device::device().used())
        Device::device().write(cs_line);
    return Return::code();
}
