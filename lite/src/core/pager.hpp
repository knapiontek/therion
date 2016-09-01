
struct Final
{
	virtual void destroy() = 0;
	Final* prev;
};

template<class Type>
struct TypeFinal
{
	virtual void destroy()
	{
		auto type = (Type*)(this + 1);
		type->~Type();
	}
	Final* prev;
};

template<class Type>
struct SeqFinal
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
	template<class Type>
	Type& acquire()
	{
		auto byte_size = int64(sizeof(Final) + sizeof(Type));
		assert(byte_size < the_page_size);

		TypeFinal<Type> final;
		final.prev = the_last_final;

		auto tail = (Final*)acquire_tail(byte_size);
		::memcpy((void*)tail, (void*)&final, sizeof(Final));
		the_last_final = tail;

		auto type = (Type*)++tail;
		new((void*)type) Type();
		return *type;
	}
	template<class Type>
	Type* acquire(int64 size)
	{
		auto byte_size = int64(sizeof(Final) + sizeof(int64) + (sizeof(Type) * size));
		assert(byte_size < the_page_size);

		SeqFinal<Type> final;
		final.prev = the_last_final;

		auto tail = (Final*)acquire_tail(byte_size);
		::memcpy((void*)tail, (void*)&final, sizeof(Final));
		the_last_final = tail;

		auto ptr_size = (int64*)++tail;
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
