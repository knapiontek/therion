
#ifndef CORE_PAGER_HPP
#define CORE_PAGER_HPP

#include "core/core/core.hpp"

NAMESPACE_BEGIN(core);

struct GenericFinal
{
	virtual void destroy() = 0;
	GenericFinal* prev;
};

template<typename Type>
struct Final
{
	virtual void destroy()
	{
		Type* type = (Type*)(this + 1);
		type->~Type();
	}
	GenericFinal* prev;
};

template<typename Type>
struct Finals
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
	GenericFinal* prev;
};

class Pager
{
public:
	Pager()
	{
		the_last = 0;
		the_tail_page = core::acquire<Page>(the_page_size);
		the_tail_page->next = 0;
		the_tail = the_tail_page->data;
	}
	~Pager()
	{
		release();
	}
	template<typename Type>
	Type* create()
	{
		uint32 byte_size = sizeof(GenericFinal) + sizeof(Type);
		assert(byte_size < the_page_size);

		Final<Type> final;
		final.prev = the_last;

		GenericFinal* gf = (GenericFinal*)acquire_tail(byte_size);
		::memcpy(gf, &final, sizeof(GenericFinal));
		the_last = gf;

		Type* type = (Type*)++gf;
		new((void*)type) Type();
		return type;
	}
	template<typename Type>
	Type* create(uint32 size)
	{
		uint32 byte_size = sizeof(GenericFinal) + sizeof(core::uint32) + (sizeof(Type) * size);
		assert(byte_size < the_page_size);

		Finals<Type> finals;
		finals.prev = the_last;

		GenericFinal* gf = (GenericFinal*)acquire_tail(byte_size);
		::memcpy(gf, &finals, sizeof(GenericFinal));
		the_last = gf;

		core::uint32* pt_size = (core::uint32*)++gf;
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
	void release()
	{
		while(the_last)
		{
			the_last->destroy();
			the_last = the_last->prev;
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
	GenericFinal* the_last;
	Page* the_tail_page;
	uint8* the_tail;
};

NAMESPACE_END(core);

#endif // CORE_PAGER_HPP
