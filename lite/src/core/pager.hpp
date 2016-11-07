
struct Final
{
	virtual void destroy() = 0;
	Final* prev;
};

template<class Type>
struct VarFinal
{
	virtual void destroy()
	{
		auto var = reinterpret_cast<Type*>(this + 1);
		var->~Type();
	}
	Final* prev;
};

template<class Type>
struct VarSeqFinal
{
	virtual void destroy()
	{
		auto data_size = reinterpret_cast<int64*>(this + 1);
		auto begin = reinterpret_cast<Type*>(data_size + 1);
		auto it = begin + *data_size;
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
		certify(byte_size < the_page_size);

		VarFinal<Type> final;
		final.prev = the_last_final;

		auto tail = reinterpret_cast<Final*>(acquire_tail(byte_size));
		::memcpy((void*)tail, (void*)&final, sizeof(Final));
		the_last_final = tail;

		auto var = reinterpret_cast<Type*>(++tail);
		new((void*)var) Type();
		return *var;
	}
	template<class Type>
	Type* acquire(int64 size)
	{
		auto byte_size = int64(sizeof(Final) + sizeof(int64) + (sizeof(Type) * size));
		certify(byte_size < the_page_size);

		VarSeqFinal<Type> final;
		final.prev = the_last_final;

		auto tail = reinterpret_cast<Final*>(acquire_tail(byte_size));
		::memcpy((void*)tail, (void*)&final, sizeof(Final));
		the_last_final = tail;

		auto data_size = (int64*)++tail;
		*data_size = size;

		auto var = reinterpret_cast<Type*>(++data_size);
		auto it = var;
		auto end = var + size;
		while(it < end)
		{
			new((void*)it) Type();
			it++;
		}
		return var;
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
private:
	static const int64 the_page_size = 0x1000;
private:
	Final* the_last_final;
	Page* the_tail_page;
	uint8* the_tail;
};
