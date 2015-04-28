
#include "core/io/output.hpp"
#include "core/core/pager.hpp"
#include "core/core/list.hpp"
#include "make/include.hpp"
#include "make/makefile.hpp"

#include "make/cs.hpp"
#include "core/io/file.hpp"
#include "core/env/console.hpp"
#include "core/core/hashmap.hpp"

class Build
{
public:
	Build() : the_include_map(0x11)
	{

	}
	void find_includes(core::String& filename)
	{
		env::Log log;
		log.debug("file: $1").arg(filename).end();

		Include& include = the_include_map.at(filename);
		if(!include.done++)
		{
			io::File file(filename);
			if(file.try_open(io::File::read_only))
			{
				core::uint32 file_size = file.size();
				core::String file_content;
				file_content.size(file_size);
				io::Decode decode = file.input();
				decode.read(file_content);

				core::String::Iterator byte_it = file_content.head();
				while(byte_it.next())
				{
					core::uint8 byte = byte_it.value();
					if('#' == byte)
					{
						include::Decode decode;
						decode.put(byte);
						while(byte_it.next() && decode.put(byte_it.value()));
						include::Model model = decode.model();
						if(model)
						{
							include::Merge merge;
							if(model->angle)
							{
								merge.append(model->angle->path);
								if(!include.angle_list.page_size())
									include.angle_list.page_size(0x11);
								include.angle_list.append(merge);
							}
							else if(model->quote)
							{
								merge.append("src/");
								merge.append(model->quote->path);
								if(!include.quote_list.page_size())
									include.quote_list.page_size(0x11);
								include.quote_list.append(merge);
							}
							else
								core::assert(false);
							log.debug(" - include: $1").arg(merge).end();
						}
					}
				}

				file.close();

				if(include.quote_list.page_size())
				{
					Include::List::Iterator st_it = include.quote_list.head();
					while(st_it.next())
					{
						core::String& value = st_it.value();
						find_includes(value);
					}
				}
			}
		}
	}
	void makefile(core::String& filename)
	{
		io::File file(filename);
		file.open(io::File::read_write);

		makefile::Decode decode;
		makefile::Encode encode(file.output());

		core::HashMap<core::String, Include>::Iterator it = the_include_map.head();
		while(it.next())
		{
			core::String& filename = it.key();
			Include& include = it.value();

			if(include.quote_list.page_size())
			{
				makefile::Path path = decode.acquire_String(filename);
				makefile::DependPlus depend_plus = decode.acquire_DependPlus();

				Include::List::Iterator quote_it = include.quote_list.head();
				while(quote_it.next())
				{
					core::String& quote = quote_it.value();
					makefile::Path path = decode.acquire_String(quote);
					makefile::Depend depend = decode.acquire_Depend(path);
					depend_plus = decode.acquire_DependPlus(depend_plus, depend);
				}

				makefile::Target target = decode.acquire_Target(path, depend_plus);
				encode.write(target);
			}
		}
	}
private:
	struct Include
	{
		typedef core::List<core::String> List;
		Include() : done(false) {}
		bool done;
		List angle_list;
		List quote_list;
	};
	core::HashMap<core::String, Include> the_include_map;
};

int main(int argc, char* argv[])
{
	env::init(argc, argv);
	try
	{
		if(3 == argc)
		{
			Build build;
			build.find_includes(argv[1]);
			build.makefile(argv[2]);
		}
		else if(4 == argc)
		{
			if(!cs_verbose.equal(argv[1]))
				env::Fail::fire(cs_input_parameters);

			env::Log::verbose() = true;

			Build build;
			build.find_includes(argv[2]);
			build.makefile(argv[3]);
		}
		else
			env::Fail::fire(cs_input_parameters);
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
