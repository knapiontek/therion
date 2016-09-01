
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
