
class RangeIterator
{
public:
    typedef int Type;
public:
    RangeIterator(Type pos) : the_pos(pos)
    {

    }
    bool operator!=(const RangeIterator& it)
    {
        return the_pos != it.the_pos;
    }
    Type operator++()
    {
        return ++the_pos;
    }
    Type operator--()
    {
        return --the_pos;
    }
    Type operator*()
    {
        return the_pos;
    }
private:
    Type the_pos;
};

template<typename Iterator>
class ReverseIterator : public Iterator
{
public:
    ReverseIterator(typename Iterator::Type pos) : Iterator(pos - 1)
    {

    }
    bool operator!=(const ReverseIterator& it)
    {
        return Iterator::operator!=(it);
    }
    typename Iterator::Type operator++()
    {
        return Iterator::operator--();
    }
    typename Iterator::Type operator--()
    {
        return Iterator::operator++();
    }
};

class Range
{
public:
    typedef RangeIterator Iterator;
public:
    Range(Iterator::Type begin, Iterator::Type end) : the_begin(begin), the_end(end)
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
    ReverseIterator<Iterator> reverse_begin()
    {
        return ReverseIterator<Iterator>(the_end);
    }
    ReverseIterator<Iterator> reverse_end()
    {
        return ReverseIterator<Iterator>(the_begin);
    }
private:
    Iterator::Type the_begin;
    Iterator::Type the_end;
};

Range range(Range::Iterator::Type begin, Range::Iterator::Type end)
{
    return Range(begin, end);
}

template<typename Forward>
class Reverse
{
public:
    Reverse(Forward& forward): the_forward(forward)
    {

    }
    ReverseIterator<typename Forward::Iterator> begin()
    {
        return the_forward.reverse_begin();
    }
    ReverseIterator<typename Forward::Iterator> end()
    {
        return the_forward.reverse_end();
    }
private:
    Forward& the_forward;
};

template<typename Forward>
Reverse<Forward> reverse(Forward&& forward)
{
    return Reverse<Forward>(forward);
}
