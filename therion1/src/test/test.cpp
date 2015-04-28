
#include "test/testservice.hpp"
#include "test/testobgen.hpp"
#include "test/testlangen.hpp"
#include "test/testio.hpp"
#include "test/testenv.hpp"
#include "test/testthread.hpp"
#include "test/testcore.hpp"

int main(int argc, char* argv[])
{
	env::init(argc, argv);
	try
	{
		test_core();
		test_thread();
		test_env();
		test_io();
		test_langen();
		test_obgen();
		test_service();
	}
	catch(env::Exception& exception)
	{
		env::Con::exception(exception);
	}
	catch(...)
	{
		core::verify(false);
	}
	return env::final() & ~env::Return::throw_alloc_exception;
}
