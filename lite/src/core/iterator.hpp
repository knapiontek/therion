
template<typename Forward>
class ReverseClass
{
public:
    ReverseClass(Forward& forward): the_forward(forward)
    {

    }
    typename Forward::Reverse begin()
    {
        return the_forward.rbegin();
    }
    typename Forward::Reverse end()
    {
        return the_forward.rend();
    }
private:
    Forward& the_forward;
};

template<typename Forward>
ReverseClass<Forward> reverse(Forward& forward)
{
    return ReverseClass<Forward>(forward);
}

template<typename Forward>
ReverseClass<Forward> reverse(Forward&& forward)
{
    return ReverseClass<Forward>(forward);
}

class RangeClass
{
public:
    class Range
    {
    public:
        Range(int64 pos) : the_pos(pos)
        {

        }
        bool operator!=(Range& it)
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
    class Reverse : public Range
    {
    public:
        Reverse(Range&& it) : Range(it)
        {

        }
        void operator++()
        {
            the_pos--;
        }
    };
public:
    RangeClass(int64 begin, int64 end) : the_begin(begin), the_end(end)
    {

    }
    Range begin()
    {
        return Range(the_begin);
    }
    Range end()
    {
        return Range(the_end);
    }
    Reverse rbegin()
    {
        return Reverse(Range(the_end - 1));
    }
    Reverse rend()
    {
        return Reverse(Range(the_begin - 1));
    }
private:
    int64 the_begin;
    int64 the_end;
};

RangeClass range(int64 begin, int64 end)
{
    return RangeClass(begin, end);
}

RangeClass range(int64 end)
{
    return RangeClass(0, end);
}
