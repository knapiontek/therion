
#include "daisy/reader.hpp"
#include "daisy/berkley.hpp"
#include "core/out.hpp"

int main(int argc, char* argv[])
{
	core::out::print_line("codegen: multi-purpose database access generator");
	if(argc >= 2)
	{
		Reader reader(argv[1]);
		if(reader.run())
		{
			Bdb writer(argv[1]);
			writer.run(reader.data());
		}
	}
	else
	{
		core::out::print_line("error: no input files");
	}
	return 0;
}

