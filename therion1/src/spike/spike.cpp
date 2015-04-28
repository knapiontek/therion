
#include "laygen/cs.hpp"
#include "core/env/env.hpp"
#include "core/env/console.hpp"

int main(int argc, char* argv[])
{
	env::init(argc, argv);
	try
	{
		bool verbose = false;
		core::String input_file;

		for(core::int32 i = 1; i < argc; i++)
		{
			if(cs_verbose_switch.equal(argv[i]))
				verbose = true;
			else if(cs_input_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				input_file = argv[i];
			}
			else
				env::Fail::fire(cs_usage);
		}
	}
	catch(env::Exception& exception)
	{
		env::Con::exception(exception);
	}
	catch(...)
	{
		env::Con::writeln(cs_unknown_exception);
	}
	return env::final();
}

