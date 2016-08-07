
template<typename Forward>
class Reverse
{
public:
    Reverse(Forward& forward): the_forward(forward)
    {

    }
    typename Forward::ReverseIterator begin()
    {
        return the_forward.reverse_begin();
    }
    typename Forward::ReverseIterator end()
    {
        return the_forward.reverse_end();
    }
private:
    Forward& the_forward;
};

template<typename Forward>
Reverse<Forward> reverse(Forward& forward)
{
    return Reverse<Forward>(forward);
}

template<typename Forward>
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
        Iterator(int pos) : the_pos(pos)
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
        int operator*()
        {
            return the_pos;
        }
    protected:
        int the_pos;
    };
    class ReverseIterator : public Iterator
    {
    public:
        ReverseIterator(Iterator&& it) : Iterator(it)
        {

        }
        void operator++()
        {
            the_pos--;
        }
    };
public:
    Range(int begin, int end) : the_begin(begin), the_end(end)
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
    ReverseIterator reverse_begin()
    {
        return ReverseIterator(Iterator(the_end - 1));
    }
    ReverseIterator reverse_end()
    {
        return ReverseIterator(Iterator(the_begin - 1));
    }
private:
    int the_begin;
    int the_end;
};

Range range(int begin, int end)
{
    return Range(begin, end);
}
