
struct OS
{
    static void var(core::String& name, core::String& value)
    {
        if(core::nil == value)
        {
            auto result = ::unsetenv(name.ascii());
            core::assert(!result);
        }
        else
        {
            auto result = ::setenv(name.ascii(), value.ascii(), true);
            core::assert(!result);
        }
    }
    static core::String var(core::String& name)
    {
        char* env = ::getenv(name.ascii());
        return env
            ? core::String(env)
            : core::nil;
    }
    static void abort()
    {
        ::abort();
    }
    static void exit(core::int64 code)
    {
        ::exit(code);
    }
    static core::int64 execute(core::String& command)
    {
        return ::system(command.ascii());
    }
};
