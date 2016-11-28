
template<class Forward>
class Reverse
{
public:
    Reverse(Forward& forward): the_forward(forward)
    {

    }
    class Forward::Reverse begin()
    {
        return the_forward.rbegin();
    }
    class Forward::Reverse end()
    {
        return the_forward.rend();
    }
private:
    Forward& the_forward;
};

template<class Forward>
Reverse<Forward> reverse(Forward& forward)
{
    return Reverse<Forward>(forward);
}

template<class Forward>
Reverse<Forward> reverse(Forward&& forward)
{
    return Reverse<Forward>(forward);
}

class Range
{
public:
    class Iterator
    {
    public:
        Iterator(int64 pos) : the_pos(pos)
        {

        }
        bool operator!=(Iterator& it)
        {
            return the_pos != it.the_pos;
        }
        void operator++()
        {
            the_pos++;
        }
        int64& operator*()
        {
            return the_pos;
        }
    protected:
        int64 the_pos;
    };
    class Reverse : public Iterator
    {
    public:
        Reverse(Iterator&& it) : Iterator(it)
        {

        }
        void operator++()
        {
            the_pos--;
        }
    };
public:
    Range(int64 begin, int64 end) : the_begin(begin), the_end(end)
    {

    }
    Iterator begin()
    {
        return Iterator(the_begin);
    }
    Iterator end()
    {
        return Iterator(the_end);
    }
    Reverse rbegin()
    {
        return Iterator(the_end - 1);
    }
    Reverse rend()
    {
        return Iterator(the_begin - 1);
    }
private:
    int64 the_begin;
    int64 the_end;
};

inline Range range(int64 begin, int64 end)
{
    return Range(begin, end);
}

inline Range range(int64 end)
{
    return Range(0, end);
}

template<class Key, class Value>
class Zip
{
public:
    class Iterator
    {
    public:
        Iterator(class Key::Iterator key, class Value::Iterator value) : the_key(key), the_value(value)
        {

        }
        typename Key::element& key()
        {
            return the_key.value();
        }
        typename Value::element& value()
        {
            return the_value.value();
        }
        bool operator!=(Iterator& it)
        {
            return the_key != it.the_key && the_value != it.the_value;
        }
        void operator++()
        {
            the_key.next();
            the_value.next();
        }
        void operator--()
        {
            the_key.prev();
            the_value.prev();
        }
        Iterator& operator*()
        {
            return *this;
        }
    protected:
        class Key::Iterator the_key;
        class Value::Iterator the_value;
    };
    class Reverse : public Iterator
    {
    public:
        Reverse(Iterator&& it) : Iterator(it)
        {

        }
        void operator++()
        {
            Iterator::operator--();
        }
    };
public:
    Zip(Key& key, Value& value) : the_key(key), the_value(value)
    {

    }
    Iterator begin()
    {
        return Iterator(the_key.begin(), the_value.begin());
    }
    Iterator end()
    {
        return Iterator(the_key.end(), the_value.end());
    }
    Reverse rbegin()
    {
        return Iterator(the_key.rend(), the_value.rend());
    }
    Reverse rend()
    {
        return Iterator(the_key.rbegin(), the_value.rbegin());
    }
private:
    Key& the_key;
    Value& the_value;
};

template<class Key, class Value>
inline Zip<Key, Value> zip(Key& key, Value& value)
{
    certify(key.size() == value.size());
    return Zip<Key, Value>(key, value);
}
