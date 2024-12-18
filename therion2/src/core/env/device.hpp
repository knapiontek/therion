
#ifndef ENV_DEVICE_HPP
#define ENV_DEVICE_HPP

#include "core/core/string.hpp"

NAMESPACE_BEGIN(env);

class Device
{
public:
	static Device& device()
	{
		static Device device;
		return device;
	};
public:
	Device() : the_used(false)
	{

	}
	void write(core::String& arg)
	{
		the_used = true;
		size_t result = ::fwrite(arg.ascii(), arg.size(), 1, stdout);
		core::verify(1 == result || !arg.size());
	}
	void writeln(core::String& arg)
	{
		the_used = true;
		size_t result = ::fwrite(cs_line.ascii(), cs_line.size(), 1, stdout);
		core::verify(1 == result);
		result = ::fwrite(arg.ascii(), arg.size(), 1, stdout);
		core::verify(1 == result || !arg.size());
	}
	void flush()
	{
		::fflush(stdout);
	}
	bool& used()
	{
		return the_used;
	}
private:
	bool the_used;
};

NAMESPACE_END(env);

#endif // ENV_DEVICE_HPP
