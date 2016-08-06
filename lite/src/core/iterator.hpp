
template<typename Forward>
class Reverse
{
public:
    class Iterator : public Forward::Iterator
    {
    public:
        Iterator(typename Forward::Iterator&& it) : Forward::Iterator(it)
        {

        }
        bool operator!=(Iterator& it)
        {
            return Forward::Iterator::operator!=(it);
        }
        typename Forward::Iterator::Type operator++()
        {
            return Forward::Iterator::operator--();
        }
        typename Forward::Iterator::Type operator--()
        {
            return Forward::Iterator::operator++();
        }
    };
public:
    Reverse(Forward& forward): the_forward(forward)
    {

    }
    Iterator begin()
    {
        return the_forward.reverse_begin();
    }
    Iterator end()
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
        typedef int Type;
    public:
        Iterator(int pos) : the_pos(pos)
        {

        }
        bool operator!=(Iterator& it)
        {
            return the_pos != it.the_pos;
        }
        int operator++()
        {
            return ++the_pos;
        }
        int operator--()
        {
            return --the_pos;
        }
        int operator*()
        {
            return the_pos;
        }
    private:
        int the_pos;
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
    Reverse<Range>::Iterator reverse_begin()
    {
        return Reverse<Range>::Iterator(Iterator(the_end - 1));
    }
    Reverse<Range>::Iterator reverse_end()
    {
        return Reverse<Range>::Iterator(Iterator(the_begin - 1));
    }
private:
    int the_begin;
    int the_end;
};

Range range(int begin, int end)
{
    return Range(begin, end);
}
