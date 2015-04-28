
#ifndef CORE_PAGER_HPP
#define CORE_PAGER_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

struct Final
{
	virtual void destroy() = 0;
	Final* prev;
};

template<typename Type>
struct ObjectFinal
{
	virtual void destroy()
	{
		Type* type = (Type*)(this + 1);
		type->~Type();
	}
	Final* prev;
};

template<typename Type>
struct SequenceFinal
{
	virtual void destroy()
	{
		core::uint32* pt_size = (core::uint32*)(this + 1);
		Type* begin = (Type*)(pt_size + 1);
		Type* it = begin + *pt_size;
		while(it > begin)
		{
			it--;
			it->~Type();
		}
	}
	Final* prev;
};

class Pager
{
public:
	Pager()
	{
		the_last_final = 0;
		the_tail_page = core::acquire<Page>(the_page_size);
		the_tail_page->next = 0;
		the_tail = the_tail_page->data;
	}
	~Pager()
	{
		release();
	}
	template<typename Type>
	Type* acquire()
	{
		uint32 byte_size = sizeof(Type);
		assert(byte_size < the_page_size);
		return (Type*)acquire_tail(byte_size);
	}
	template<typename Type>
	Type* acquire(uint32 size)
	{
		uint32 byte_size = sizeof(Type) * size;
		assert(byte_size < the_page_size);
		return (Type*)acquire_tail(byte_size);
	}
	template<typename Type>
	Type* acquire_nil()
	{
		uint32 byte_size = sizeof(Type);
		assert(byte_size < the_page_size);
		Type* type = (Type*)acquire_tail(byte_size);
		::bzero(type, byte_size);
		return type;
	}
	template<typename Type>
	Type* acquire_nil(uint32 size)
	{
		uint32 byte_size = sizeof(Type) * size;
		assert(byte_size < the_page_size);
		Type* type = (Type*)acquire_tail(byte_size);
		::bzero(type, byte_size);
		return type;
	}
	template<typename Type>
	Type* acquire_class()
	{
		uint32 byte_size = sizeof(Final) + sizeof(Type);
		assert(byte_size < the_page_size);

		ObjectFinal<Type> final;
		final.prev = the_last_final;

		Final* f = (Final*)acquire_tail(byte_size);
		::memcpy(f, &final, sizeof(Final));
		the_last_final = f;

		Type* type = (Type*)++f;
		new((void*)type) Type();
		return type;
	}
	template<typename Type>
	Type* acquire_class(uint32 size)
	{
		uint32 byte_size = sizeof(Final) + sizeof(core::uint32) + (sizeof(Type) * size);
		assert(byte_size < the_page_size);

		SequenceFinal<Type> final;
		final.prev = the_last_final;

		Final* f = (Final*)acquire_tail(byte_size);
		::memcpy(f, &final, sizeof(Final));
		the_last_final = f;

		core::uint32* pt_size = (core::uint32*)++f;
		*pt_size = size;

		Type* type = (Type*)++pt_size;
		Type* it = type;
		Type* end = type + size;
		while(it < end)
		{
			new((void*)it) Type();
			it++;
		}
		return type;
	}
	void release()
	{
		while(the_last_final)
		{
			the_last_final->destroy();
			the_last_final = the_last_final->prev;
		}
		while(the_tail_page)
		{
			Page* page = the_tail_page;
			the_tail_page = the_tail_page->next;
			core::release<Page>(page);
		}
	}
private:
	class Page
	{
	public:
		Page* next;
		uint8 data[1];
	};
private:
	uint8* acquire_tail(uint32 byte_size)
	{
		uint8* tail = the_tail;
		the_tail += byte_size;
		if(the_tail > (core::uint8*)the_tail_page + the_page_size)
		{
			Page* next = the_tail_page;
			the_tail_page = core::acquire<Page>(the_page_size);
			the_tail_page->next = next;
			tail = the_tail = the_tail_page->data;
			the_tail += byte_size;
		}
		return tail;
	}
	uint8* acquire_tail(uint8* pt, uint32 byte_size)
	{
		uint8* tail = the_tail;
		the_tail = pt + byte_size;
		if(the_tail > (core::uint8*)the_tail_page + the_page_size)
		{
			Page* next = the_tail_page;
			the_tail_page = core::acquire<Page>(the_page_size);
			the_tail_page->next = next;
			::memcpy(the_tail_page->data, pt, tail - pt);
			pt = the_tail = the_tail_page->data;
			the_tail += byte_size;
		}
		return pt;
	}
private:
	static const uint32 the_page_size = 0x1000;
private:
	Final* the_last_final;
	Page* the_tail_page;
	uint8* the_tail;
};

NAMESPACE_END(core);

#endif // CORE_PAGER_HPP

