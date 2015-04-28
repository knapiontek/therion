
#ifndef ENV_ENV_HPP
#define ENV_ENV_HPP

#include "core/env/cs.hpp"
#include "core/env/device.hpp"
#include "core/env/exception.hpp"
#include "core/thread/thread.hpp"
#include "core/core/format.hpp"
#include <signal.h>

NAMESPACE_BEGIN(env);

class Return
{
public:
	enum
	{
		log_fatal = 0x1,
		log_err = 0x2,
		fail = 0x4,
		throw_alloc_exception = 0x8,
		signal = 0x10
	};
public:
	static core::int32& code()
	{
		static core::int32 code = 0;
		return code;
	}
};

class Log : public core::FormatClass<Log>
{
public:
	static bool& verbose()
	{
		static bool verbose = false;
		return verbose;
	}
public:
	Log()
	{
		core::Seq<core::String>& seq = thread::Thread::current().context_seq();
		the_size = seq.size();
	}
	~Log()
	{
		core::Seq<core::String>& seq = thread::Thread::current().context_seq();
		if(seq.size() > the_size)
			seq.size(the_size);
	}
	Log& fatal(core::String& format)
	{
		Return::code() |= Return::log_fatal;
		core::FormatClass<Log>::format(format);
		the_type = env::cs_F;
		return *this;
	}
	Log& err(core::String& format)
	{
		Return::code() |= Return::log_err;
		core::FormatClass<Log>::format(format);
		the_type = env::cs_E;
		return *this;
	}
	Log& warn(core::String& format)
	{
		core::FormatClass<Log>::format(format);
		the_type = env::cs_W;
		return *this;
	}
	Log& info(core::String& format)
	{
		core::FormatClass<Log>::format(format);
		the_type = env::cs_I;
		return *this;
	}
	Log& debug(core::String& format)
	{
		core::FormatClass<Log>::format(verbose() ? format : core::String::nil());
		the_type = env::cs_D;
		return *this;
	}
	void end()
	{
		if(verbose() || the_type != env::cs_D)
		{
			if(Device::device().used())
				Device::device().write(cs_line);
			Device::device().write(the_type);
			core::String& arg = core::FormatClass<Log>::end();
			Device::device().write(arg);
			Device::device().flush();
			core::Seq<core::String>& seq = thread::Thread::current().context_seq();
			if(the_size < seq.size())
				seq.at(the_size) = arg;
			else
				seq.append(arg);
		}
	}
private:
	void endln()
	{

	}
private:
	core::String the_type;
	core::uint32 the_size;
};

class Fail : public core::FormatClass<Fail>
{
public:
	static void fire(core::String& message)
	{
		Return::code() |= Return::fail;
		env::Exception exception;
		exception.message() = message;
		core::xchange(exception.context_seq(), thread::Thread::current().context_seq());
		throw exception;
	}
public:
	Fail(core::String& format)
	{
		core::FormatClass<Fail>::format(format);
	}
	void fire()
	{
		Return::code() |= Return::fail;
		env::Exception exception;
		exception.message() = core::FormatClass<Fail>::end();
		core::xchange(exception.context_seq(), thread::Thread::current().context_seq());
		throw exception;
	}
private:
	void end()
	{

	}
	void endln()
	{

	}
};

class Signal
{
public:
	static void signal()
	{
		::signal(SIGABRT, handler);
		::signal(SIGFPE, handler);
		::signal(SIGILL, handler);
		::signal(SIGINT, handler);
		::signal(SIGSEGV, handler);
		::signal(SIGTERM, handler);
	}
private:
	class RecurrenceBarrier
	{
	public:
		~RecurrenceBarrier()
		{
			__sync_fetch_and_sub(cnt(), 1);
		}
		bool operator++(int)
		{
			return __sync_fetch_and_add(cnt(), 1);
		}
	private:
		static core::uint16* cnt()
		{
			static __thread core::uint16 cnt = 0;
			return &cnt;
		}
	};
private:
	static void handler(int id)
	{
		RecurrenceBarrier cnt;
		if(cnt++) return;

		Return::code() |= Return::signal;
		env::Exception exception;
		switch(id)
		{
		case SIGABRT:
			exception.message() = cs_abort;
			break;
		case SIGFPE:
			exception.message() = cs_fpe;
			break;
		case SIGILL:
			exception.message() = cs_ill;
			break;
		case SIGINT:
			exception.message() = cs_interrupt;
			break;
		case SIGSEGV:
			exception.message() = cs_segv;
			break;
		case SIGTERM:
			exception.message() = cs_terminate;
			break;
		default:
			core::assert(false);
		}
		core::xchange(exception.context_seq(), thread::Thread::current().context_seq());
		throw exception;
	}
};

class Handler : public core::Handler
{
public:
	static void handler()
	{
		static Handler handler;
		core::Handler::handler() = &handler;
	}
public:
	void throw_alloc_exception()
	{
		// ::malloc not allowed in here!
		Return::code() |= Return::throw_alloc_exception;
		env::Exception exception;
		exception.message() = cs_allocation_exception;
		core::xchange(exception.context_seq(), thread::Thread::current().context_seq());
		throw exception;
	}
	void print_assert(const char* file_name, int line_no, const char* content)
	{
		core::String message = core::Format(cs_assert)
			.arg(content)
			.arg(file_name)
			.arg(line_no)
			.end();
		core::uint32 pos = message.size();

		Runtime::Iterator it = Runtime::backtrace();
		while(it.next())
		{
			Runtime::Frame& frame = it.value();
			core::String st_frame = core::Format(cs_frame)
				.arg(frame.function_name)
				.arg(frame.file_name)
				.arg(frame.line)
				.end();
			message.copy_in(pos, st_frame);
		}

		message.size(pos);
		Device::device().write(message);
	}
};

inline void init(int argc, char* argv[])
{
	Signal::signal();
	Handler::handler();
	thread::Thread::main(argv[0]);
}

inline core::int32 final()
{
	if(Device::device().used())
		Device::device().write(cs_line);
	return Return::code();
}

NAMESPACE_END(env);

#endif // ENV_ENV_HPP
