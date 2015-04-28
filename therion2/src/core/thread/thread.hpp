
#ifndef THREAD_THREAD_HPP
#define THREAD_THREAD_HPP

#include "core/core/string.hpp"
#include "core/core/seq.hpp"
#include <pthread.h>

NAMESPACE_BEGIN(thread);

class Runnable
{
public:
	typedef core::Shared<Runnable> share;
	typedef core::Managed<Runnable> manage;
public:
	virtual ~Runnable()
	{

	}
	virtual void run()
	{
		core::assert(false);
	}
	virtual void interrupt()
	{
		core::assert(false);
	}
};

class Thread : Runnable
{
public:
	typedef core::Shared<Thread> share;
public:
	static Thread& current()
	{
		return *thread();
	}
	static void main(core::String& name)
	{
		static Thread main(name);
		core::assert(!thread::Thread::thread());
		thread::Thread::thread() = &main;
	}
	static bool active()
	{
		return thread()->the_active;
	}
public:
	Thread(core::String& name = core::nil)
	{
		the_name = name;
		the_pthread = 0;
	}
	~Thread()
	{
		join();
	}
	core::String& name()
	{
		return the_name;
	}
	core::Seq<core::String>& context_seq()
	{
		return the_context_seq;
	}
	void start()
	{
		join();
		the_active = true;
		the_runnable = this;
		pthread_attr_t attr;
		::pthread_attr_init(&attr);
		::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		::pthread_create(&the_pthread, &attr, Thread::run, this);
		::pthread_attr_destroy(&attr);
	}
	void start(Runnable& runnable)
	{
		join();
		the_active = true;
		the_runnable = &runnable;
		pthread_attr_t attr;
		::pthread_attr_init(&attr);
		::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		::pthread_create(&the_pthread, &attr, Thread::run, this);
		::pthread_attr_destroy(&attr);
	}
	void interrupt()
	{
		if(the_active)
		{
			the_active = false;
			the_runnable->interrupt();
		}
	}
	void join()
	{
		if(the_pthread)
		{
			core::int32 exit_code = 0;
			::pthread_join(the_pthread, (void**)&exit_code);
			the_pthread = 0;
		}
	}
private:
	static void* run(void* arg)
	{
		thread() = (Thread*)arg;
		thread()->the_runnable->run();
		thread()->the_active = false;
		return 0;
	}
	static Thread*& thread()
	{
		static __thread Thread* thread = 0;
		return thread;
	}
private:
	bool the_active;
	Runnable* the_runnable;
	pthread_t the_pthread;
	core::String the_name;
	core::Seq<core::String> the_context_seq;
};

NAMESPACE_END(thread);

#endif // THREAD_THREAD_HPP
