
#ifndef THREAD_LOCKABLE_QUEUE_HPP
#define THREAD_LOCKABLE_QUEUE_HPP

#include "core/thread/event.hpp"
#include "core/core/queue.hpp"

NAMESPACE_BEGIN(thread);

template<typename Element>
class LockableQueue
{
public:
	typedef core::Shared<LockableQueue> share;
	typedef core::Managed<LockableQueue> manage;
public:
	LockableQueue()
	{

	}
	LockableQueue(core::uint32 page_size)
	{
		the_queue.page_size(page_size);
	}
	void page_size(core::uint32 page_size)
	{
		the_queue.page_size(page_size);
	}
	void interrupt(bool active = false)
	{
		Locker locker(the_event);
		the_event.interrupt(active);
	}
	void push(const Element& element)
	{
		Locker locker(the_event);
		the_queue.push(element);
		the_event.notify();
	}
	bool pop(Element& element, bool lock = false)
	{
		Locker locker(the_event);
		while(lock && the_queue.is_empty() && the_event.wait());
		return the_queue.pop(element);
	}
	core::uint32 size()
	{
		Locker locker(the_event);
		return the_queue.size();
	}
private:
	LockableEvent the_event;
	core::Queue<Element> the_queue;
};

NAMESPACE_END(thread);

#endif // THREAD_LOCKABLE_QUEUE_HPP
