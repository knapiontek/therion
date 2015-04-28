
#ifndef THREAD_EVENT_HPP
#define THREAD_EVENT_HPP

#include "core/core/share.hpp"
#include "core/core/manage.hpp"
#include <pthread.h>
#include <sys/time.h>

NAMESPACE_BEGIN(thread);

class Lock
{
	friend class Locker;
	friend class Event;
public:
	Lock()
	{
		::pthread_mutex_init(&the_mutex, 0);
	}
	~Lock()
	{
		::pthread_mutex_destroy(&the_mutex);
	}
	bool try_acquire()
	{
		return !::pthread_mutex_trylock(&the_mutex);
	}
	void acquire()
	{
		::pthread_mutex_lock(&the_mutex);
	}
	void release()
	{
		::pthread_mutex_unlock(&the_mutex);
	}
private:
	pthread_mutex_t the_mutex;
};

class Event
{
	friend class Locker;
public:
	typedef core::Shared<Event> share;
	typedef core::Managed<Event> manage;
public:
	Event()
	{

	}
	Event(Lock& lock)
	{
		Event::lock(lock);
	}
	~Event()
	{
		::pthread_cond_destroy(&the_cond);
	}
	void lock(Lock& lock)
	{
		the_mutex = &lock.the_mutex;
		the_active = true;
		::pthread_cond_init(&the_cond, 0);
	}
	void interrupt(bool active = false)
	{
		core::assert(::pthread_mutex_trylock(the_mutex));
		the_active = active;
		::pthread_cond_broadcast(&the_cond);
	}
	void notify()
	{
		core::assert(::pthread_mutex_trylock(the_mutex));
		::pthread_cond_signal(&the_cond);
	}
	void notify_all()
	{
		core::assert(::pthread_mutex_trylock(the_mutex));
		::pthread_cond_broadcast(&the_cond);
	}
	bool wait()
	{
		core::assert(::pthread_mutex_trylock(the_mutex));
		return the_active && !::pthread_cond_wait(&the_cond, the_mutex) && the_active;
	}
	bool wait(core::uint32 msecs)
	{
		core::assert(::pthread_mutex_trylock(the_mutex));
		struct timeval tv;
		struct timespec ts;
		::gettimeofday(&tv, 0);
		ts.tv_nsec = (tv.tv_usec + (msecs % 1000) * 1000) * 1000;
		ts.tv_sec = tv.tv_sec + (msecs / 1000) + (ts.tv_nsec / 1000000000);
		ts.tv_nsec %= 1000000000;
		return the_active && !::pthread_cond_timedwait(&the_cond, the_mutex, &ts) && the_active;
	}
private:
	bool the_active;
	pthread_cond_t the_cond;
	pthread_mutex_t* the_mutex;
};

class LockableEvent
{
	friend class Locker;
public:
	typedef core::Shared<LockableEvent> share;
	typedef core::Managed<LockableEvent> manage;
public:
	LockableEvent()
	{
		the_active = true;
		::pthread_mutex_init(&the_mutex, 0);
		::pthread_cond_init(&the_cond, 0);
	}
	~LockableEvent()
	{
		::pthread_cond_destroy(&the_cond);
		::pthread_mutex_destroy(&the_mutex);
	}
	void interrupt(bool active = false)
	{
		core::assert(::pthread_mutex_trylock(&the_mutex));
		the_active = active;
		::pthread_cond_broadcast(&the_cond);
	}
	bool try_acquire()
	{
		return !::pthread_mutex_trylock(&the_mutex);
	}
	void acquire()
	{
		::pthread_mutex_lock(&the_mutex);
	}
	void release()
	{
		::pthread_mutex_unlock(&the_mutex);
	}
	void notify()
	{
		core::assert(::pthread_mutex_trylock(&the_mutex));
		::pthread_cond_signal(&the_cond);
	}
	void notify_all()
	{
		core::assert(::pthread_mutex_trylock(&the_mutex));
		::pthread_cond_broadcast(&the_cond);
	}
	bool wait()
	{
		core::assert(::pthread_mutex_trylock(&the_mutex));
		return the_active && !::pthread_cond_wait(&the_cond, &the_mutex) && the_active;
	}
	bool wait(core::uint32 msecs)
	{
		core::assert(::pthread_mutex_trylock(&the_mutex));
		struct timeval tv;
		struct timespec ts;
		::gettimeofday(&tv, 0);
		ts.tv_nsec = (tv.tv_usec + (msecs % 1000) * 1000) * 1000;
		ts.tv_sec = tv.tv_sec + (msecs / 1000) + (ts.tv_nsec / 1000000000);
		ts.tv_nsec %= 1000000000;
		return the_active && !::pthread_cond_timedwait(&the_cond, &the_mutex, &ts) && the_active;
	}
private:
	bool the_active;
	pthread_cond_t the_cond;
	pthread_mutex_t the_mutex;
};

class Locker
{
public:
	Locker(Lock& lock)
	{
		the_mutex = &lock.the_mutex;
		::pthread_mutex_lock(the_mutex);
	}
	Locker(Event& event)
	{
		the_mutex = event.the_mutex;
		::pthread_mutex_lock(the_mutex);
	}
	Locker(LockableEvent& event)
	{
		the_mutex = &event.the_mutex;
		::pthread_mutex_lock(the_mutex);
	}
	~Locker()
	{
		::pthread_mutex_unlock(the_mutex);
	}
private:
	pthread_mutex_t* the_mutex;
};

NAMESPACE_END(thread);

#endif // THREAD_EVENT_HPP
