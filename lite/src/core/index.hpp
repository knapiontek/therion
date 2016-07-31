
#ifndef CORE_INDEX_HPP
#define CORE_INDEX_HPP

#include "core/core/string.hpp"

NAMESPACE_BEGIN(core);

struct PrivateIndex
{
	uint32 hash(uint8* key, uint32 size)
	{
		uint8* pos = (uint8*)key;
		uint8* tail = pos + size;
		uint32 hash = 0;
		while(pos < tail)
		{
			hash += *pos++;
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}
	uint32 hash(int8 key)
	{
		return (uint)key;
	}
	uint32 hash(int16 key)
	{
		return (uint)key;
	}
	uint32 hash(int32 key)
	{
		return (uint)key;
	}
	uint32 hash(int64 key)
	{
		return (uint)(key >> 31 ^ key);
	}
	uint32 hash(uint8 key)
	{
		return (uint)key;
	}
	uint32 hash(uint16 key)
	{
		return (uint)key;
	}
	uint32 hash(uint32 key)
	{
		return (uint)key;
	}
	uint32 hash(uint64 key)
	{
		return (uint)(key >> 31 ^ key);
	}
	uint32 hash(float32 key)
	{
		return (uint)key;
	}
	uint32 hash(float64 key)
	{
		return (uint)key;
	}
	uint32 hash(float128 key)
	{
		return (uint)key;
	}
	uint32 hash(String& key)
	{
		uint8* pos = key.data();
		uint8* tail = pos + key.size();
		uint32 top, hash = 0;
		while(pos < tail)
		{
			hash = (hash << 4) + *pos++;
			top = hash & 0xF0000000;
			if(top)
				hash ^= top >> 23;
			hash &= ~top;
		}
		return hash;
	}
	int32 compare(uint8* arg1, uint32 size1, uint8* arg2, uint32 size2)
	{
		uint32 size = min(size1, size2);
		int32 result = ::memcmp(arg1, arg2, size);
		return result ? result : (size1 - size2);
	}
	int32 compare(int8 arg1, int8 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(int16 arg1, int16 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(int32 arg1, int32 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(int64 arg1, int64 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(uint8 arg1, uint8 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(uint16 arg1, uint16 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(uint32 arg1, uint32 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(uint64 arg1, uint64 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(float32 arg1, float32 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(float64 arg1, float64 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(float128 arg1, float128 arg2)
	{
		return (int32)(arg1 - arg2);
	}
	int32 compare(String& arg1, String& arg2)
	{
		return arg1.compare(arg2);
	}
};

template<typename Cast>
struct Index : PrivateIndex
{
	template<typename Type>
	uint32 hash(Type* key)
	{
		return (uint32)key;
	}
	template<typename Type>
	uint32 hash(Type& key)
	{
		Cast& cast = (Cast&)key;
		return PrivateIndex::hash(cast);
	}
	template<typename Type>
	uint32 hash(Shared<Type>& key)
	{
		Cast& cast = (Cast&)key;
		return PrivateIndex::hash(cast);
	}
	template<typename Type>
	uint32 hash(Managed<Type>& key)
	{
		Cast& cast = (Cast&)key;
		return PrivateIndex::hash(cast);
	}
	template<typename Type>
	int32 compare(Type* arg1, Type* arg2)
	{
		return (int32)(arg1 - arg2);
	}
	template<typename Type>
	int32 compare(Type& arg1, Type& arg2)
	{
		Cast& cast1 = (Cast&)arg1;
		Cast& cast2 = (Cast&)arg2;
		return PrivateIndex::compare(cast1, cast2);
	}
	template<typename Type>
	int32 compare(Shared<Type>& arg1, Shared<Type>& arg2)
	{
		Cast& cast1 = (Cast&)arg1;
		Cast& cast2 = (Cast&)arg2;
		return PrivateIndex::compare(cast1, cast2);
	}
	template<typename Type>
	int32 compare(Managed<Type>& arg1, Managed<Type>& arg2)
	{
		Cast& cast1 = (Cast&)arg1;
		Cast& cast2 = (Cast&)arg2;
		return PrivateIndex::compare(cast1, cast2);
	}
};

NAMESPACE_END(core);

#endif // CORE_INDEX_HPP

