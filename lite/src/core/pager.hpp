
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
		auto type = (Type*)(this + 1);
		type->~Type();
	}
	Final* prev;
};

template<typename Type>
struct SequenceFinal
{
	virtual void destroy()
	{
		auto ptr_size = (int64*)(this + 1);
		auto begin = (Type*)(ptr_size + 1);
		auto it = begin + *ptr_size;
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
	Type& acquire()
	{
		auto byte_size = sizeof(Type);
		assert(byte_size < the_page_size);
		return *(Type*)acquire_tail(byte_size);
	}
	template<typename Type>
	Type* acquire(int64 size)
	{
		auto byte_size = sizeof(Type) * size;
		assert(byte_size < the_page_size);
		return (Type*)acquire_tail(byte_size);
	}
	template<typename Type>
	Type* acquire_nil()
	{
		auto byte_size = sizeof(Type);
		assert(byte_size < the_page_size);
		auto type = (Type*)acquire_tail(byte_size);
		::bzero(type, byte_size);
		return type;
	}
	template<typename Type>
	Type* acquire_nil(int64 size)
	{
		auto byte_size = sizeof(Type) * size;
		assert(byte_size < the_page_size);
		auto type = (Type*)acquire_tail(byte_size);
		::bzero(type, byte_size);
		return type;
	}
	template<typename Type>
	Type* acquire_class()
	{
		auto byte_size = sizeof(Final) + sizeof(Type);
		assert(byte_size < the_page_size);

		ObjectFinal<Type> final;
		final.prev = the_last_final;

		auto f = (Final*)acquire_tail(byte_size);
		::memcpy(f, &final, sizeof(Final));
		the_last_final = f;

		auto type = (Type*)++f;
		new((void*)type) Type();
		return type;
	}
	template<typename Type>
	Type* acquire_class(int64 size)
	{
		auto byte_size = sizeof(Final) + sizeof(int64) + (sizeof(Type) * size);
		assert(byte_size < the_page_size);

		SequenceFinal<Type> final;
		final.prev = the_last_final;

		auto f = (Final*)acquire_tail(byte_size);
		::memcpy(f, &final, sizeof(Final));
		the_last_final = f;

		auto ptr_size = (int64*)++f;
		*ptr_size = size;

		auto type = (Type*)++ptr_size;
		auto it = type;
		auto end = type + size;
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
			auto page = the_tail_page;
			the_tail_page = the_tail_page->next;
			core::release<Page>(page);
		}
	}
private:
	struct Page
	{
		Page* next;
		uint8 data[1];
	};
private:
	uint8* acquire_tail(int64 byte_size)
	{
		auto tail = the_tail;
		the_tail += byte_size;
		if(the_tail > (uint8*)the_tail_page + the_page_size)
		{
			auto next = the_tail_page;
			the_tail_page = core::acquire<Page>(the_page_size);
			the_tail_page->next = next;
			tail = the_tail = the_tail_page->data;
			the_tail += byte_size;
		}
		return tail;
	}
	uint8* acquire_tail(uint8* pt, int64 byte_size)
	{
		auto tail = the_tail;
		the_tail = pt + byte_size;
		if(the_tail > (uint8*)the_tail_page + the_page_size)
		{
			auto next = the_tail_page;
			the_tail_page = core::acquire<Page>(the_page_size);
			the_tail_page->next = next;
			::memcpy(the_tail_page->data, pt, tail - pt);
			pt = the_tail = the_tail_page->data;
			the_tail += byte_size;
		}
		return pt;
	}
private:
	static const int64 the_page_size = 0x1000;
private:
	Final* the_last_final;
	Page* the_tail_page;
	uint8* the_tail;
};
