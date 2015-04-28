
#include "langen/prototype.hpp"
#include "langen/encode.hpp"
#include "langen/cs.hpp"
#include "core/env/console.hpp"

int main(int argc, char* argv[])
{
	env::init(argc, argv);
	try
	{
		bool verbose = false;
		bool no_machine = false;
		core::String input_file;
		core::String output_file;

		for(core::int32 i = 1; i < argc; i++)
		{
			if(cs_verbose_switch.equal(argv[i]))
				verbose = true;
			else if(cs_no_machine_switch.equal(argv[i]))
				no_machine = true;
			else if(cs_input_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				input_file = argv[i];
			}
			else if(cs_output_switch.equal(argv[i]))
			{
				if(++i == argc)
					env::Fail::fire(cs_usage);
				output_file = argv[i];
			}
			else
				env::Fail::fire(cs_usage);
		}

		Model model;

		model.level = Model::machine;
		if(verbose)
			model.level = Model::verbose;
		if(no_machine)
			model.level = Model::none;

		env::Log::verbose() = verbose;

		Prototype::read(model, input_file);
		Encode::write(model, output_file);
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

