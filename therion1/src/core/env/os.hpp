
#ifndef ENV_OS_HPP
#define ENV_OS_HPP

#include "core/core/string.hpp"

NAMESPACE_BEGIN(env);

struct OS
{
	static void var(core::String& name, core::String& value)
	{
		if(value.equal(core::String()))
		{
			core::uint32 result = ::unsetenv(name.ascii());
			core::assert(!result);
		}
		else
		{
			core::uint32 result = ::setenv(name.ascii(), value.ascii(), true);
			core::assert(!result);
		}
	}
	static core::String var(core::String& name)
	{
		return ::getenv(name.ascii());
	}
	static void abort()
	{
		::abort();
	}
	static void exit(core::uint32 code)
	{
		::exit(code);
	}
	static core::uint32 execute(core::String& command)
	{
		return ::system(command.ascii());
	}
};

NAMESPACE_END(env);

#endif // ENV_OS_HPP
