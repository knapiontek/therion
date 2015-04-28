
#ifndef TEST_THREAD_HPP
#define TEST_THREAD_HPP

#include "test/cs.hpp"
#include "core/thread/threadpool.hpp"

class SynchThread : thread::Thread
{
public:
	static void test()
	{
		SynchThread thread;
		thread.start();
	}
private:
	SynchThread() : thread::Thread(cs_synch_thread)
	{

	}
	void start()
	{
		thread::Thread::start();
		for(core::uint32 i = 0; i < 1000; i++)
		{
			thread::Locker locker(the_synch);
			the_synch.notify_all();
		}
		the_synch.acquire();
		the_synch.interrupt();
		the_synch.release();
		join();
	}
	void run()
	{
		try
		{
			thread::Locker locker(the_synch);
			while(the_synch.wait());
		}
		catch(env::Exception& exception)
		{
			env::Con::exception(exception);
		}
		catch(...)
		{
			core::verify(false);
		}
	}
private:
	thread::LockableEvent the_synch;
};

class Counter
{
public:
	typedef core::Shared<Counter> share;
public:
	Counter() : the_value(0)
	{

	}
	void delta(core::int32 delta)
	{
		thread::Locker locker(the_lock);
		the_value += delta;
	}
	core::int32& value()
	{
		return the_value;
	}
private:
	thread::Lock the_lock;
	core::int32 the_value;
};

class Consumer : public thread::Runnable
{
public:
	void init(thread::LockableQueue<core::uint32>& queue, Counter& cnt)
	{
		the_queue = queue;
		the_cnt = cnt;
		the_arg = true;
	}
private:
	void run()
	{
		try
		{
			core::uint32 i = 0;
			core::verify(the_queue->pop(i, true));
			the_cnt->delta(-i);
			core::verify(the_arg);
			the_arg = false;
		}
		catch(env::Exception& exception)
		{
			env::Con::exception(exception);
		}
		catch(...)
		{
			core::verify(false);
		}
	}
private:
	thread::LockableQueue<core::uint32>::share the_queue;
	Counter::share the_cnt;
	bool the_arg;
};

class Producer : thread::Thread
{
public:
	static void test()
	{
		Producer thread;
		thread.start();
	}
private:
	Producer() : thread::Thread(cs_producer_thread)
	{

	}
	void start()
	{
		the_queue.page_size(9);
		the_seq.size(1000);
		the_pool.size(100);
		thread::Thread::start();
		core::Seq<Consumer>::Iterator it = the_seq.head();
		while(it.next())
		{
			Consumer& consumer = it.value();
			consumer.init(the_queue, the_cnt);
			the_pool.start(consumer);
		}
		thread::LockableEvent event;
		thread::Locker locker(event);
		event.wait(100);
		the_queue.interrupt();
		the_pool.interrupt();
		join();
		the_pool.join();
		core::verify(!the_cnt.value());
	}
private:
	void run()
	{
		try
		{
			for(core::uint32 i = 0; i < the_seq.size(); i++)
			{
				the_queue.push(i);
				the_cnt.delta(i);
			}
		}
		catch(env::Exception& exception)
		{
			env::Con::exception(exception);
		}
		catch(...)
		{
			core::verify(false);
		}
	}
private:
	core::Seq<Consumer> the_seq;
	thread::ThreadPool the_pool;
	thread::LockableQueue<core::uint32> the_queue;
	Counter the_cnt;
};

inline void test_thread()
{
	SynchThread::test();
	Producer::test();
}

#endif // TEST_THREAD_HPP
