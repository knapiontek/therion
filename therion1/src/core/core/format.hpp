
#ifndef CORE_FORMAT_HPP
#define CORE_FORMAT_HPP

#include "core/core/string.hpp"

NAMESPACE_BEGIN(core);

template<class FormatReturn>
class FormatClass
{
public:
	void format(String& format)
	{
		the_format = format;
		the_result = String::nil();
		the_cnt = 0;
	}
	String& end()
	{
		if(the_format.is_empty())
			return join(false);
		else
			return merge(false);
	}
	String& endln()
	{
		if(the_format.is_empty())
			return join(true);
		else
			return merge(true);
	}
	FormatReturn& arg(String& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = arg.data();
		item.size = arg.size();
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const char* arg)
	{
		assert(arg && (the_cnt < (sizeof(the_items) / sizeof(Item))));
		Item& item = the_items[the_cnt++];
		item.pt = (uint8*)arg;
		item.size = ::strlen(arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(uint8* arg, uint32 size)
	{
		assert(arg && (the_cnt < (sizeof(the_items) / sizeof(Item))));
		Item& item = the_items[the_cnt++];
		item.pt = arg;
		item.size = size;
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const bool& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const int8& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const int16& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const int32& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const int64& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const uint8& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const uint16& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const uint32& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const uint64& arg)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const float32& arg, uint8 precision = float32_precision)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg, precision);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const float64& arg, uint8 precision = float64_precision)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg, precision);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(const float128& arg, uint8 precision = float128_precision)
	{
		assert(the_cnt < (sizeof(the_items) / sizeof(Item)));
		Item& item = the_items[the_cnt++];
		item.pt = item.data;
		item.size = sizeof(item.data);
		Convert::it(item.pt, item.size, arg, precision);
		return *(FormatReturn*)this;
	}
	FormatReturn& arg(Region& arg)
	{
		assert(arg.data && (the_cnt < (sizeof(the_items) / sizeof(Item))));
		Item& item = the_items[the_cnt++];
		item.pt = arg.data;
		item.size = arg.size;
		return *(FormatReturn*)this;
	}
private:
	String& merge(bool new_line)
	{
		static const core::uint8 hex2index[] =	{
			0, 1, 2, 3, 4, 5, 6, 7, 8,
			-1, -1, -1, -1, -1, -1, -1,
			9,
			0xA, 0xB, 0xC, 0xD, 0xE
		};
		uint32 pos = 0;
		uint32 last = 0;
		if(new_line) the_result.copy_in(pos, "\n");
		String::Iterator it = the_format.head();
		while(it.next())
		{
			if('$' == it.value())
			{
				the_result.copy_in(pos, the_format.data() + last, it.position() - last);
				last = it.position() + 1; // skip %
				if(it.next() && '$' != it.value())
				{
					last++; // skip digit
					uint32 index = it.value() - '1';
					if(index < sizeof(hex2index))
					{
						index = hex2index[index];
						assert(index < the_cnt);
						Item& item = the_items[index];
						the_result.copy_in(pos, item.pt, item.size);
					}
					else assert(false);
				}
			}
		}
		the_result.copy_in(pos, the_format.data() + last, it.position() - last);
		the_result.size(pos);
		return the_result;
	}
	String& join(bool new_line)
	{
		uint32 size = 0;
		for(uint32 i = 0; i < the_cnt; i++)
			size += the_items[i].size;
		the_result.size(size);

		uint32 pos = 0;
		for(uint32 i = 0; i < the_cnt; i++)
		{
			Item& item = the_items[i];
			the_result.copy_in(pos, item.pt, item.size);
		}

		assert(size == pos);
		return the_result;
	}
private:
	String the_format;
	String the_result;
	uint32 the_cnt;
	struct Item
	{
		uint8 data[int8_max];
		uint8* pt;
		uint32 size;
	} the_items[0xF]; // hex 1 - F
};

class Format : public FormatClass<Format>
{
public:
	Format()
	{
		FormatClass<Format>::format(String::nil());
	}
	Format(String& arg)
	{
		FormatClass<Format>::format(arg);
	}
};

NAMESPACE_END(core);

#endif // CORE_FORMAT_HPP
