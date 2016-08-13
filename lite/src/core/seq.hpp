
template< typename Element, class SeqIndex = Index<Element>>
class Seq
{
public:
    typedef Shared<Seq> share;
    typedef Managed<Seq> manage;
public:
    class Iterator
    {
        friend class Seq;
    public:
        bool is_head()
        {
            return the_elem < the_seq->the_head;
        }
        bool is_tail()
        {
            return the_elem == the_seq->the_tail;
        }
        bool next()
        {
            return ++the_elem < the_seq->the_tail;
        }
        bool prev()
        {
            return the_elem-- > the_seq->the_head;
        }
        int64 position()
        {
            return the_elem - the_seq->the_head;
        }
        Element& value()
        {
            return *the_elem;
        }
        bool operator!=(Iterator& it)
        {
            return the_elem != it.the_elem;
        }
        void operator++()
        {
            the_elem++;
        }
        void operator--()
        {
            the_elem--;
        }
        Iterator& operator*()
        {
            return *this;
        }
    private:
        Iterator(Element* elem, Seq* seq)
        {
            the_elem = elem;
            the_seq = seq;
        }
    private:
        Element* the_elem;
        Seq* the_seq;
    };
    class Reverse : public Iterator
    {
        friend class Seq;
    public:
        void operator++()
        {
            Iterator::operator--();
        }
    private:
        Reverse(Iterator&& it) : Iterator(it)
        {

        }
    };
    class Find
    {
        friend class Seq;
    public:
        bool next()
        {
            return (++the_elem < the_high);
        }
        int64 position()
        {
            return the_elem - the_seq->the_head;
        }
        Element& value()
        {
            return *the_elem;
        }
        friend bool operator==(const Nil&, const Find& arg)
        {
            return !arg.the_low;
        }
        friend bool operator!=(const Nil&, const Find& arg)
        {
            return arg.the_low;
        }
        friend bool operator==(const Find& arg, const Nil&)
        {
            return !arg.the_low;
        }
        friend bool operator!=(const Find& arg, const Nil&)
        {
            return arg.the_low;
        }
    private:
        Find(Element* low, Element* high, Seq* seq)
        {
            the_elem = low;
            the_low = low;
            the_high = high;
            the_seq = seq;
        }
    private:
        Element* the_elem;
        Element* the_low;
        Element* the_high;
        Seq* the_seq;
    };
public:
    Seq()
    {
        init();
    }
    Seq(int64 size)
    {
        init();
        Seq::size(size);
    }
    ~Seq()
    {
        erase_all();
    }
    Iterator head()
    {
        return Iterator(the_head - 1, this);
    }
    Iterator tail()
    {
        return Iterator(the_tail, this);
    }
    Iterator begin()
    {
        return Iterator(the_head, this);
    }
    Iterator end()
    {
        return tail();
    }
    Reverse rbegin()
    {
        return Iterator(the_tail - 1, this);
    }
    Reverse rend()
    {
        return head();
    }
    void sort()
    {
        if(the_head)
        {
            const auto heap_size = 32; // limited to 4294967296 elements
            auto i = 1;
            Element* lheap[heap_size];
            Element* hheap[heap_size];
            auto lbond = the_head;
            auto hbond = the_tail - 1;
            while(i)
            {
                auto bond = hbond - lbond;
                if(2 <= bond)
                {
                    // find middle as a pivot
                    bond >>= 1;
                    auto pos = lbond + bond;
                    // sort lbond, pos, hbond - prevent pathology
                    if(0 < the_index.compare(*lbond, *hbond))
                        xchange(*lbond, *hbond);
                    if(0 > the_index.compare(*pos, *lbond))
                        xchange(*pos, *lbond);
                    else if(0 < the_index.compare(*pos, *hbond))
                        xchange(*pos, *hbond);
                    auto size = sizeof(Element);
                    uint8 pivot[size];
                    ::memcpy(pivot, pos, size);
                    auto l = lbond + 1;
                    auto h = hbond - 1;
                    while(l < h)
                    {
                        while(0 > the_index.compare(*l, *(Element*)pivot))
                            l++;
                        while(0 < the_index.compare(*h, *(Element*)pivot))
                            h--;
                        if(l < h)
                        {
                            xchange(*l, *h);
                            l++;
                            h--;
                        }
                    }
                    if(h - lbond < hbond - l)
                    {
                        lheap[i] = l;
                        hheap[i] = hbond;
                        hbond = h;
                    }
                    else
                    {
                        lheap[i] = lbond;
                        hheap[i] = h;
                        lbond = l;
                    }
                    i++;
                }
                else
                {
                    if(1 == bond && 0 < the_index.compare(*lbond, *hbond))
                        xchange(*lbond, *hbond);
                    i--;
                    lbond = lheap[i];
                    hbond = hheap[i];
                }
            }
        }
    }
    Find find(const Element& arg)
    {
        auto lbond = 0;
        auto hbond = the_tail - 1 - the_head;
        while(lbond <= hbond)
        {
            auto pos = (lbond + hbond) >> 1;
            auto dir = the_index.compare((Element&)arg, the_head[pos]);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
            {
                auto low = the_head + pos - 1;
                auto high = the_head + pos + 1;
                while(low >= the_head && !the_index.compare((Element&)arg, *low))
                    low--;
                while(high < the_tail && !the_index.compare((Element&)arg, *high))
                    high++;
                return Find(low, high, this);
            }
        }
        return Find(0, 0, this);
    }
    Element* content()
    {
        return the_head;
    }
    void capacity(int64 bond)
    {
        auto tail = the_tail - the_head;
        assert(bond > tail);
        the_head = core::acquire<Element>(the_head, sizeof(Element) * bond);
        the_tail = the_head + tail;
        the_bond = the_head + bond;
    }
    int64 capacity()
    {
        return the_bond - the_head;
    }
    void squeeze()
    {
        if(the_bond > the_tail)
        {
            auto bond = the_tail - the_head;
            the_head = core::acquire<Element>(the_head, sizeof(Element) * bond);
            the_tail = the_bond = the_head + bond;
        }
    }
    void size(int64 size)
    {
        auto elem = the_tail;
        while(elem > the_head + size)
        {
            elem--;
            elem->~Element();
        }
        auto tail = the_tail - the_head;
        auto bond = the_bond - the_head;
        if(size > bond)
        {
            the_head = core::acquire<Element>(the_head, sizeof(Element) * size);
            the_bond = the_head + size;
        }
        elem = the_head + tail;
        while(elem < the_head + size)
        {
            new((void*)elem) Element();
            elem++;
        }
        the_tail = the_head + size;
    }
    int64 size()
    {
        return the_tail - the_head;
    }
    bool is_empty()
    {
        return the_tail == the_head;
    }
    void erase_all()
    {
        if(the_head)
        {
            auto elem = the_tail;
            while(elem > the_head)
            {
                elem--;
                elem->~Element();
            }
            release<Element>(the_head);
            init();
        }
    }
    template<class Iterator>
    void erase_by_iterator(Iterator it)
    {
        if(it.next())
        {
            assert(it.value() < size());
            auto elem = the_head + it.value();
            elem->~Element();
            auto l = elem;
            auto h = elem + 1;
            while(it.next())
            {
                assert(it.value() < size());
                elem = the_head + it.value();
                elem->~Element();
                if(elem != h)
                {
                    assert(l < h);
                    ::memmove((void*)l, h, sizeof(Element) * (elem - h));
                    l += elem - h;
                    h = elem;
                }
                h++;
            }
            ::memmove((void*)l, h, sizeof(Element) * (the_tail - elem - 1));
            the_tail -= h - l;
        }
    }
    Element& at(int64 pos)
    {
        assert(pos < size());
        return the_head[pos];
    }
    Element& operator[](int64 pos)
    {
        return at(pos);
    }
    Element& put(int64 pos, const Element& arg)
    {
        acquire_tail();
        assert(pos < size());
        auto elem = the_head + pos;
        ::memmove((void*)(elem + 1), elem, sizeof(Element) * (the_tail - 1 - elem));
        new((void*)elem) Element(arg);
        return *elem;
    }
    void erase(int64 pos)
    {
        assert(pos < size());
        auto elem = the_head + pos;
        elem->~Element();
        the_tail--;
        ::memmove((void*)elem, elem + 1, sizeof(Element) * (the_tail - elem));
    }
    void erase_by_tail(int64 pos)
    {
        assert(pos < size());
        auto elem = the_head + pos;
        elem->~Element();
        the_tail--;
        ::memcpy((void*)elem, the_tail, sizeof(Element));
    }
    Element& append()
    {
        auto elem = acquire_tail();
        new((void*)elem) Element();
        return *elem;
    }
    Element& append(const Element& arg)
    {
        auto elem = acquire_tail();
        new((void*)elem) Element(arg);
        return *elem;
    }
    int64 search(const Element& arg)
    {
        auto lbond = 0;
        auto hbond = the_tail - 1 - the_head;
        while(lbond <= hbond)
        {
            auto pos = (lbond + hbond) >> 1;
            auto dir = the_index.compare((Element&)arg, the_head[pos]);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
                return pos;
        }
        return int64_nil;
    }
    Shared<Element> lookup(const Element& arg)
    {
        auto lbond = 0;
        auto hbond = the_tail - 1 - the_head;
        while(lbond <= hbond)
        {
            auto pos = (lbond + hbond) >> 1;
            auto& result = the_head[pos];
            auto dir = the_index.compare((Element&)arg, result);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
                return result;
        }
        return nil;
    }
    template<class Pager>
    Element& acquire(const Element& arg, Pager& pager)
    {
        auto lbond = 0;
        auto hbond = the_tail - 1 - the_head;
        while(lbond <= hbond)
        {
            auto pos = (lbond + hbond) >> 1;
            auto& result = the_head[pos];
            auto dir = the_index.compare((Element&)arg, result);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
                return result;
        }
        acquire_tail();
        auto elem = the_head + lbond;
        ::memmove((void*)(elem + 1), elem, sizeof(Element) * (the_tail - 1 - elem));
        new((void*)elem) Element(pager.append());
        return *elem;
    }
    bool put(const Element& arg, bool unique = true)
    {
        auto lbond = 0;
        auto hbond = the_tail - 1 - the_head;
        while(lbond <= hbond)
        {
            auto pos = (lbond + hbond) >> 1;
            auto dir = the_index.compare((Element&)arg, the_head[pos]);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else if(!unique)
            {
                lbond = pos + 1;
                break;
            }
            else
                return false;
        }
        acquire_tail();
        auto elem = the_head + lbond;
        ::memmove((void*)(elem + 1), elem, sizeof(Element) * (the_tail - 1 - elem));
        new((void*)elem) Element(arg);
        return true;
    }
    int64 erase(const Element& arg, bool unique = true)
    {
        auto lbond = 0;
        auto hbond = the_tail - 1 - the_head;
        while(lbond <= hbond)
        {
            auto pos = (lbond + hbond) >> 1;
            auto dir = the_index.compare((Element&)arg, the_head[pos]);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else if(!unique)
            {
                auto low = the_head + pos - 1;
                auto high = the_head + pos + 1;
                while(low >= the_head && !the_index.compare((Element&)arg, *low))
                    low--;
                while(high < the_tail && !the_index.compare((Element&)arg, *high))
                    high++;
                low++;
                high--;
                auto erased = high - low + 1;
                auto it = low;
                while(it <= high)
                {
                    it->~Element();
                    it++;
                }
                ::memmove((void*)low, high + 1, sizeof(Element) * (the_tail - high - 1));
                the_tail -= erased;
                return erased;
            }
            else
            {
                auto elem = the_head + pos;
                elem->~Element();
                the_tail--;
                ::memmove((void*)elem, elem + 1, sizeof(Element) * (the_tail - elem));
                return 1;
            }
        }
        return 0;
    }
private:
    Element* acquire_tail()
    {
        if(the_tail == the_bond)
        {
            auto tail = the_tail - the_head;
            auto bond = the_bond - the_head;
            bond = bond * 15 / 10 + 5;
            the_head = core::acquire<Element>(the_head, sizeof(Element) * bond);
            the_tail = the_head + tail;
            the_bond = the_head + bond;
        }
        return the_tail++;
    }
    void init()
    {
        the_head = 0;
        the_tail = 0;
        the_bond = 0;
    }
private:
    Element* the_head;
    Element* the_tail;
    Element* the_bond;
    SeqIndex the_index;
};
