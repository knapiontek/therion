
#ifndef TEST_OBGEN_HPP
#define TEST_OBGEN_HPP

#include <db.h>
#include "core/io/file.hpp"
#include "test/sample/sample.hpp"

class Pipe : public io::Input, io::Output
{
public:
	Pipe()
	{
		the_pos_in = 0;
		the_pos_out = 0;
		the_content.size(0x4FF);
	}
	io::Input& input()
	{
		return *this;
	}
	io::Output& output()
	{
		return *this;
	}
private:
	void input(core::uint8* data, core::uint32 size)
	{
		the_pos_out = 0;
		the_content.copy_out(the_pos_in, data, size);
	}
	void skip(core::uint32 size)
	{
		the_pos_in += size;
	}
	void output(core::uint8* data, core::uint32 size)
	{
		the_pos_in = 0;
		the_content.copy_in(the_pos_out, data, size);
	}
private:
	core::uint32 the_pos_in;
	core::uint32 the_pos_out;
	core::String the_content;
};

inline void test_obgen()
{
	Pipe pipe;
	sample::Encode encode(pipe.output());
	sample::Decode decode(pipe.input());
	sample::Index index;

	sample::Country country;
	country.name = "Ireland";

	sample::Town town;
	town.name = "Dublin";
	town.uint8_id = 1;
	town.country.name = country.name;

	sample::Street street;
	street.name = "Shelbourn Road";
	street.town.name = town.name;
	street.town.uint8_id = town.uint8_id;

	sample::Address address;
	address.no = 1;
	address.street.name = street.name;
	address.street.town.uint8_id = street.town.uint8_id;

	sample::Person worker;
	worker.first_name = "Alexander";
	worker.last_name = "The Great";
	worker.uint16_id = 1;
	worker.uint64_id = 1;
	worker.address.no = address.no;
	worker.address.street.name = address.street.name;
	worker.address.street.town.uint8_id = 0;

	sample::Factory factory_out;
	factory_out.name = core::Format("Therion Farm $1").arg(core::randomize() % 10).end();
	factory_out.country_seq.append(country);
	factory_out.town_seq.append(town);
	factory_out.worker_seq.append(worker);

	encode.write(country);
	decode.read(country);
	encode.write(town);
	decode.read(town);
	encode.write(street);
	decode.read(street);
	encode.write(address);
	decode.read(address);
	encode.write(worker);
	decode.read(worker);

	// factory
	sample::Factory factory_in1;
	encode.write(factory_out);
	decode.read(factory_in1);
	core::assert(!index.compare(factory_in1, factory_out));

	// factory persist
	sample::Factory factory_in2;
	sample::Persist persist;
	persist.open("home");

	sample::FactorySeq& factory_seq = persist.factory_seq();
	core::uint32 factory_id = factory_seq.put(factory_out);
	factory_seq.set(factory_id, factory_out);

::printf("\nid: %d\n", factory_id);
	factory_seq.get(factory_id, factory_in2);
	factory_seq.set(factory_id, factory_in2);
	factory_seq.get(factory_id, factory_in2);

	sample::Iterator<sample::Factory> it = factory_seq.find_all_sort_by_name();
	while(it.prev())
	{
		core::uint32 id;
		sample::Factory factory;
		it.key(id);
		it.value(factory);
		::printf("\nhello: %d %s", id, factory.name.ascii());
	}
	it.close();

	factory_seq.erase(factory_id);
	factory_seq.put(factory_out);
	persist.close();
	core::assert(!index.compare(factory_in2, factory_out));
}

#endif // TEST_OBGEN_HPP
