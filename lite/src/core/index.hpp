
struct InternalIndex
{
    int64 hash(uint8* key, int64 size)
    {
        auto pos = (uint8*)key;
        auto tail = pos + size;
        int64 hash = 0;
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
    int64 hash(int8 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(int16 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(int32 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(int64 key)
    {
        return (int64)(key >> 31 ^ key);
    }
    int64 hash(uint8 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(uint16 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(uint32 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(uint64 key)
    {
        return (int64)(key >> 31 ^ key);
    }
    int64 hash(float32 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(float64 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(float128 key)
    {
        return static_cast<int64>(key);
    }
    int64 hash(String& key)
    {
        auto pos = key.data();
        auto tail = pos + key.size();
        int64 top, hash = 0;
        while(pos < tail)
        {
            hash = (hash << 4) + *pos++;
            top = hash & 0xF000000000000000;
            if(top)
                hash ^= top >> 23;
            hash &= ~top;
        }
        return hash;
    }
    int64 compare(uint8* arg1, int64 size1, uint8* arg2, int64 size2)
    {
        int64 size = min(size1, size2);
        int32 result = ::memcmp(arg1, arg2, size);
        return result ? result : (size1 - size2);
    }
    int64 compare(int8 arg1, int8 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(int16 arg1, int16 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(int32 arg1, int32 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(int64 arg1, int64 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(uint8 arg1, uint8 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(uint16 arg1, uint16 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(uint32 arg1, uint32 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(uint64 arg1, uint64 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(float32 arg1, float32 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(float64 arg1, float64 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(float128 arg1, float128 arg2)
    {
        return (int64)(arg1 - arg2);
    }
    int64 compare(String& arg1, String& arg2)
    {
        return arg1.compare(arg2);
    }
};

template<class Cast>
struct Index : InternalIndex
{
    template<class Type>
    int64 hash(Type* key)
    {
        return static_cast<int64>(key);
    }
    template<class Type>
    int64 hash(Type& key)
    {
        auto& cast = reinterpret_cast<Cast&>(key);
        return InternalIndex::hash(cast);
    }
    template<class Type>
    int64 hash(Shared<Type>& key)
    {
        auto& cast = reinterpret_cast<Cast&>(key);
        return InternalIndex::hash(cast);
    }
    template<class Type>
    int64 hash(Managed<Type>& key)
    {
        auto& cast = reinterpret_cast<Cast&>(key);
        return InternalIndex::hash(cast);
    }
    template<class Type>
    int64 compare(Type* arg1, Type* arg2)
    {
        return (int64)(arg1 - arg2);
    }
    template<class Type>
    int64 compare(Type& arg1, Type& arg2)
    {
        auto& cast1 = reinterpret_cast<Cast&>(arg1);
        auto& cast2 = reinterpret_cast<Cast&>(arg2);
        return InternalIndex::compare(cast1, cast2);
    }
    template<class Type>
    int64 compare(Shared<Type>& arg1, Shared<Type>& arg2)
    {
        auto& cast1 = reinterpret_cast<Cast&>(arg1);
        auto& cast2 = reinterpret_cast<Cast&>(arg2);
        return InternalIndex::compare(cast1, cast2);
    }
    template<class Type>
    int64 compare(Managed<Type>& arg1, Managed<Type>& arg2)
    {
        auto& cast1 = reinterpret_cast<Cast&>(arg1);
        auto& cast2 = reinterpret_cast<Cast&>(arg2);
        return InternalIndex::compare(cast1, cast2);
    }
};
