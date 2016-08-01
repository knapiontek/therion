
#include "test/testio.hpp"
#include "test/testenv.hpp"
#include "test/testcore.hpp"

int main(int argc, char* argv[])
{
    env::init(argc, argv);
    try
    {
        test_core();
        test_env();
        test_io();
    }
    catch(env::Exception& exception)
    {
        env::Con::exception(exception);
    }
    catch(...)
    {
        core::verify(false);
    }
    return env::final() & ~env::Return::throw_alloc_exception; // intentionally thrown
}

