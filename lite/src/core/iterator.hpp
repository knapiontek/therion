
template<typename Forward>
class Reverse
{
public:
    Reverse(Forward& forward): the_forward(forward)
    {

    }
    typename Forward::ReverseLoopIterator begin()
    {
        return the_forward.rbegin();
    }
    typename Forward::ReverseLoopIterator end()
    {
        return the_forward.rend();
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
    class LoopIterator
    {
    public:
        LoopIterator(int32 pos) : the_pos(pos)
        {

        }
        bool operator!=(LoopIterator& it)
        {
            return the_pos != it.the_pos;
        }
        void operator++()
        {
            the_pos++;
        }
        int32 operator*()
        {
            return the_pos;
        }
    protected:
        int32 the_pos;
    };
    class ReverseLoopIterator : public LoopIterator
    {
    public:
        ReverseLoopIterator(LoopIterator&& it) : LoopIterator(it)
        {

        }
        void operator++()
        {
            the_pos--;
        }
    };
public:
    Range(int32 begin, int32 end) : the_begin(begin), the_end(end)
    {

    }
    LoopIterator begin()
    {
        return LoopIterator(the_begin);
    }
    LoopIterator end()
    {
        return LoopIterator(the_end);
    }
    ReverseLoopIterator rbegin()
    {
        return ReverseLoopIterator(LoopIterator(the_end - 1));
    }
    ReverseLoopIterator rend()
    {
        return ReverseLoopIterator(LoopIterator(the_begin - 1));
    }
private:
    int32 the_begin;
    int32 the_end;
};

Range range(int32 begin, int32 end)
{
    return Range(begin, end);
}

Range range(int32 end)
{
    return Range(0, end);
}
