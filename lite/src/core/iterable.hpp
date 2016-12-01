
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

template<class Iterable1, class Iterable2>
class Zip
{
public:
    class Iterator
    {
    public:
        Iterator(class Iterable1::Iterator&& it1, class Iterable2::Iterator&& it2) : the_it1(it1), the_it2(it2)
        {

        }
        class Iterable1::Iterator& it1()
        {
            return the_it1;
        }
        class Iterable2::Iterator& it2()
        {
            return the_it2;
        }
        bool operator!=(Iterator& it)
        {
            return the_it1 != it.the_it1;
        }
        void operator++()
        {
            the_it1.operator++();
            the_it2.operator++();
        }
        void operator--()
        {
            the_it1.operator--();
            the_it2.operator--();
        }
        Iterator& operator*()
        {
            return *this;
        }
    protected:
        class Iterable1::Iterator the_it1;
        class Iterable2::Iterator the_it2;
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
    Zip(Iterable1& it1, Iterable2& it2) : the_it1(it1), the_it2(it2)
    {

    }
    Iterator begin()
    {
        return Iterator(the_it1.begin(), the_it2.begin());
    }
    Iterator end()
    {
        return Iterator(the_it1.end(), the_it2.end());
    }
    Reverse rbegin()
    {
        return Iterator(the_it1.rbegin(), the_it2.rbegin());
    }
    Reverse rend()
    {
        return Iterator(the_it1.rend(), the_it2.rend());
    }
private:
    Iterable1& the_it1;
    Iterable2& the_it2;
};

template<class Iterable1, class Iterable2>
inline Zip<Iterable1, Iterable2> zip(Iterable1& it1, Iterable2& it2)
{
    certify(it1.size() == it2.size());
    return Zip<Iterable1, Iterable2>(it1, it2);
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
