
inline void test_exception()
{
    try
    {
        core::float64 var_float64 = 9.999995;
        env::Exception exception;
        exception.message() = core::Format("exception test %1")
            .arg(var_float64, 5)
            .end();
        throw exception;
    }
    catch(env::Exception& exception)
    {
        core::verify(exception.message().equal("exception test 10.00000"));
    }
    catch(...)
    {
        core::verify(false);
    }
}

inline void test_utils()
{
    core::String cs_name = "name";
    core::String cs_value = "value";

    env::Time time;
    env::OS::var(cs_name, cs_value);
    auto result = env::OS::var(cs_name);
    core::verify(result.equal(cs_value));
    env::OS::var(cs_name, core::nil);
    time.mark();
    return;
    env::Log log;
    log.debug("test: %1") % 1 % core::end;
    env::Con::print(result);
    throw env::Format::exception(result);
    env::OS::exit(0);
    env::OS::execute("sh");
    env::OS::abort();
}

inline void test_env()
{
    test_exception();
    test_utils();
}
