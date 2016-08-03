
core::String cs_message = cs("exception test 10.00000");
core::String cs_exception_test = cs("exception test $1");
core::String cs_name = cs("name");
core::String cs_value = cs("value");
core::String cs_sh = cs("sh");

inline void test_exception()
{
    try
    {
        const core::float64 var_float64 = 9.999995;
        env::Exception exception;
        exception.message() = core::Format(cs_exception_test)
            .arg(var_float64, 5)
            .end();
        throw exception;
    }
    catch(env::Exception& exception)
    {
        core::verify(cs_message.equal(exception.message()));
    }
    catch(...)
    {
        core::verify(false);
    }
}

inline void test_utils()
{
    env::Time time;
    env::OS::var(cs_name, cs_value);
    core::String result = env::OS::var(cs_name);
    core::verify(result.equal(cs_value));
    env::OS::var(cs_name, core::nil);
    time.mark();
    return;
    env::Log log;
    env::Con::write(result);
    env::Fail::fire(result);
    env::OS::exit(0);
    env::OS::execute(cs_sh);
    env::OS::abort();
}

inline void test_env()
{
    test_exception();
    test_utils();
}
