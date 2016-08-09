
template<typename Forward>
class Reverse
{
public:
    Reverse(Forward& forward): the_forward(forward)
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
    class Loop
    {
    public:
        Loop(int64 pos) : the_pos(pos)
        {

        }
        bool operator!=(Loop& it)
        {
            return the_pos != it.the_pos;
        }
        void operator++()
        {
            the_pos++;
        }
        int64 operator*()
        {
            return the_pos;
        }
    protected:
        int64 the_pos;
    };
    class Reverse : public Loop
    {
    public:
        Reverse(Loop&& it) : Loop(it)
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
    Loop begin()
    {
        return Loop(the_begin);
    }
    Loop end()
    {
        return Loop(the_end);
    }
    Reverse rbegin()
    {
        return Reverse(Loop(the_end - 1));
    }
    Reverse rend()
    {
        return Reverse(Loop(the_begin - 1));
    }
private:
    int64 the_begin;
    int64 the_end;
};

Range range(int64 begin, int64 end)
{
    return Range(begin, end);
}

Range range(int64 end)
{
    return Range(0, end);
}
