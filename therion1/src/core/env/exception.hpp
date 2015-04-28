
#ifndef ENV_EXCEPTION_HPP
#define ENV_EXCEPTION_HPP

#include "core/env/runtime.hpp"
#include "core/core/string.hpp"
#include "core/core/seq.hpp"

NAMESPACE_BEGIN(env);

class Exception
{
public:
	core::String& message()
	{
		return the_message;
	}
	Runtime& runtime()
	{
		return the_runtime;
	}
	core::Seq<core::String>& context_seq()
	{
		return the_context_seq;
	}
private:
	Runtime the_runtime;
	core::String the_message;
	core::Seq<core::String>::manage the_context_seq;
};

NAMESPACE_END(env);

#endif // ENV_EXCEPTION_HPP
