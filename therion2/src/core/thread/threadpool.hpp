
#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "core/thread/event.hpp"
#include "core/thread/thread.hpp"
#include "core/thread/lockablequeue.hpp"
#include "core/core/seq.hpp"

NAMESPACE_BEGIN(thread);

class ThreadPool : Runnable
{
public:
	ThreadPool() : the_start_event(the_lock), the_ready_event(the_lock)
	{

	}
	ThreadPool(core::uint32 size) : the_start_event(the_lock), the_ready_event(the_lock)
	{
		ThreadPool::size(size);
	}
	void size(core::uint32 size)
	{
		the_idle_cnt = 0;
		core::uint32 page_size = 3 + size / 3;
		the_runnable_queue.page_size(page_size);
		the_thread_queue.page_size(page_size);
		the_pool_seq.size(size);
		core::Seq<Thread>::Iterator it = the_pool_seq.head();
		while(it.next())
			the_thread_queue.push(it.value());
	}
	void interrupt(bool active = false)
	{
		Locker locker(the_lock);
		the_start_event.interrupt(active);
		the_ready_event.interrupt(active);
	}
	void join()
	{
		core::Seq<Thread>::Iterator it = the_pool_seq.head();
		while(it.next())
			it.value().join();
	}
	bool start(Runnable& runnable)
	{
		Locker locker(the_lock);
		Thread::share thread;
		if(the_idle_cnt)
		{
			the_idle_cnt--;
			the_start_event.notify();
		}
		else if(the_thread_queue.pop(thread))
			thread->start(*this);
		else if(!the_ready_event.wait())
			return false;
		else if(the_idle_cnt)
		{
			the_idle_cnt--;
			the_start_event.notify();
		}
		else if(the_thread_queue.pop(thread))
			thread->start(*this);
		the_runnable_queue.push(runnable);
		return true;
	}
private:
	bool wait(Runnable::share& runnable)
	{
		Locker locker(the_lock);
		if(the_runnable_queue.pop(runnable))
			return true;
		the_idle_cnt++;
		the_ready_event.notify();
		while(the_start_event.wait(10000))
		{
			if(the_runnable_queue.pop(runnable))
				return true;
		}
		the_idle_cnt--;
		the_thread_queue.push(Thread::current());
		the_ready_event.notify();
		return false;
	}
	void run()
	{
		Runnable::share runnable;
		while(wait(runnable))
			runnable->run();
	}
private:
	Lock the_lock;
	Event the_start_event;
	Event the_ready_event;
	core::uint32 the_idle_cnt;
	core::Queue<Runnable::share> the_runnable_queue;
	core::Queue<Thread::share> the_thread_queue;
	core::Seq<Thread> the_pool_seq;
};

NAMESPACE_END(thread);

#endif // THREAD_POOL_HPP
