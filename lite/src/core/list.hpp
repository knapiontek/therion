
template< typename Element, class ListIndex = Index<Element> >
class List
{
    struct Cursor;
public:
    typedef Shared<List> share;
    typedef Managed<List> manage;
public:
    class Iterator
    {
        friend class List;
    public:
        bool begin()
        {
            return the_cr.page == the_list->the_head && the_cr.elem == *the_cr.page;
        }
        bool end()
        {
            return the_cr.elem == the_list->the_tail.elem;
        }
        bool next()
        {
            the_cr.elem++;
            if(the_cr.elem == the_hbond)
            {
                if(the_cr.elem == the_list->the_tail.elem)
                    return false;
                the_cr.page++;
                the_cr.elem = *the_cr.page;
                the_lbond = the_cr.elem;
                the_hbond = (the_cr.page != the_list->the_tail.page)
                    ? *the_cr.page + the_list->the_page_size
                    : the_list->the_tail.elem;
            }
            return true;
        }
        bool prev()
        {
            if(the_cr.elem == the_lbond)
            {
                if(the_cr.page == the_list->the_head)
                    return false;
                the_cr.page--;
                the_cr.elem = *the_cr.page + the_list->the_page_size;
                the_lbond = *the_cr.page;
                the_hbond = the_cr.elem;
            }
            the_cr.elem--;
            return true;
        }
        uint32 position()
        {
            return the_list->the_page_size * (the_cr.page - the_list->the_head) + (the_cr.elem - the_lbond);
        }
        Element& value()
        {
            return *the_cr.elem;
        }
    private:
        Iterator(Element** page, Element* elem, Element* lbond, Element* hbond, List* list)
        {
            the_cr.page = page;
            the_cr.elem = elem;
            the_lbond = lbond;
            the_hbond = hbond;
            the_list = list;
        }
    private:
        Cursor the_cr;
        Element* the_lbond;
        Element* the_hbond;
        List* the_list;
    };
    class Find
    {
        friend class List;
    public:
        bool next()
        {
            the_cr.elem++;
            if(the_cr.elem == the_hbond)
            {
                if(the_cr.elem == the_high.elem)
                    return false;
                the_cr.page++;
                the_cr.elem = *the_cr.page;
                the_lbond = the_cr.elem;
                the_hbond = (the_cr.page != the_high.page)
                    ? *the_cr.page + the_list->the_page_size
                    : the_high.elem;
            }
            return true;
        }
        uint32 position()
        {
            return the_list->the_page_size * (the_cr.page - the_list->the_head) + (the_cr.elem - the_lbond);
        }
        Element& value()
        {
            return *the_cr.elem;
        }
        friend bool operator==(const Nil&, const Find& arg)
        {
            return !arg.the_low.elem;
        }
        friend bool operator!=(const Nil&, const Find& arg)
        {
            return arg.the_low.elem;
        }
        friend bool operator==(const Find& arg, const Nil&)
        {
            return !arg.the_low.elem;
        }
        friend bool operator!=(const Find& arg, const Nil&)
        {
            return arg.the_low.elem;
        }
    private:
        Find(Cursor& low, Cursor& high, List* list)
        {
            the_list = list;
            the_low = low;
            the_high = high;
            the_high.elem++;
            the_cr = the_low;
            the_cr.elem--;
            the_lbond = the_low.elem;
            the_hbond = (the_low.page != the_high.page)
                ? *the_low.page + the_list->the_page_size
                : the_high.elem;
        }
        Find()
        {
            ::bzero(this, sizeof(Find));
            the_cr.elem--;
        }
    private:
        Cursor the_cr;
        Cursor the_low;
        Cursor the_high;
        Element* the_lbond;
        Element* the_hbond;
        List* the_list;
    };
public:
    List()
    {
        the_page_size = 0;
        init();
    }
    List(uint32 page_size)
    {
        assert(page_size);
        the_page_size = page_size;
        init();
    }
    ~List()
    {
        if(the_page_size)
            erase_all();
    }
    Iterator begin()
    {
        assert(the_page_size);
        Element* hbond = (the_head != the_tail.page)
            ? *the_head + the_page_size
            : the_tail.elem;
        return Iterator(the_head, *the_head - 1, *the_head, hbond, this);
    }
    Iterator end()
    {
        assert(the_page_size);
        return Iterator(the_tail.page, the_tail.elem, *the_tail.page, the_tail.elem, this);
    }
    void sort()
    {
        assert(the_page_size);
        if(the_bond)
        {
            const uint32 heap_size = 32; // limited to 4294967296 elements
            uint32 i = 1;
            Cursor lheap[heap_size], hheap[heap_size];
            Cursor lbond, hbond, low, pos, high;
            lbond.page = the_head;
            lbond.elem = *the_head;
            hbond = the_tail;
            prev(hbond);
            while(i)
            {
                uint32 bond = distance(hbond, lbond);
                if(2 <= bond)
                {
                    // find middle as a pivot
                    bond >>= 1;
                    bond += lbond.elem - *lbond.page;
                    pos.page = lbond.page + (bond / the_page_size);
                    pos.elem = *pos.page + (bond % the_page_size);
                    // sort lbond, pos, hbond - prevent pathology
                    if(0 < the_index.compare(*lbond.elem, *hbond.elem))
                        xchange(*lbond.elem, *hbond.elem);
                    if(0 > the_index.compare(*pos.elem, *lbond.elem))
                        xchange(*pos.elem, *lbond.elem);
                    else if(0 < the_index.compare(*pos.elem, *hbond.elem))
                        xchange(*pos.elem, *hbond.elem);
                    uint32 size = sizeof(Element);
                    uint8 pivot[size];
                    ::memcpy(pivot, pos.elem, size);
                    low = lbond;
                    high = hbond;
                    next(low);
                    prev(high);
                    while(low.page < high.page || (low.page == high.page && low.elem < high.elem))
                    {
                        while(0 > the_index.compare(*low.elem, *(Element*)pivot))
                            next(low);
                        while(0 < the_index.compare(*high.elem, *(Element*)pivot))
                            prev(high);
                        if(low.page < high.page || (low.page == high.page && low.elem < high.elem))
                        {
                            xchange(*low.elem, *high.elem);
                            next(low);
                            prev(high);
                        }
                    }
                    if(distance(high, lbond) < distance(hbond, low))
                    {
                        lheap[i] = low;
                        hheap[i] = hbond;
                        hbond = high;
                    }
                    else
                    {
                        lheap[i] = lbond;
                        hheap[i] = high;
                        lbond = low;
                    }
                    i++;
                }
                else
                {
                    if(1 == bond && 0 < the_index.compare(*lbond.elem, *hbond.elem))
                        xchange(*lbond.elem, *hbond.elem);
                    i--;
                    lbond = lheap[i];
                    hbond = hheap[i];
                }
            }
        }
    }
    Find find(const Element& arg)
    {
        assert(the_page_size);
        int32 lbond = 0;
        int32 hbond = size() - 1;
        while(lbond <= hbond)
        {
            uint32 pos = (lbond + hbond) >> 1;
            int32 dir = the_index.compare((Element&)arg, at(pos));
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
            {
                Cursor low, high;
                convert(low, pos);
                high = low;
                while((low.elem != *the_head) && !the_index.compare((Element&)arg, *low.elem))
                    prev(low);
                while((high.elem != the_tail.elem - 1) && !the_index.compare((Element&)arg, *high.elem))
                    next(high);
                if(the_index.compare((Element&)arg, *low.elem))
                    next(low);
                if(the_index.compare((Element&)arg, *high.elem))
                    prev(high);
                return Find(low, high, this);
            }
        }
        return Find();
    }
    void page_size(uint32 page_size)
    {
        assert(!the_page_size && page_size);
        the_page_size = page_size;
    }
    uint32 page_size()
    {
        return the_page_size;
    }
    uint32 size()
    {
        assert(the_page_size);
        return the_page_size * (the_tail.page - the_head) + (the_tail.elem - *the_tail.page);
    }
    bool is_empty()
    {
        assert(the_page_size);
        return !the_bond;
    }
    void erase_all()
    {
        assert(the_page_size);
        if(the_bond)
        {
            the_bond = *the_tail.page;
            while(the_tail.elem > the_bond)
            {
                the_tail.elem--;
                the_tail.elem->~Element();
            }
            release<Element>(*the_tail.page);
            the_tail.page--;
            while(the_tail.page >= the_head)
            {
                the_tail.elem = *the_tail.page + the_page_size;
                the_bond = *the_tail.page;
                while(the_tail.elem > the_bond)
                {
                    the_tail.elem--;
                    the_tail.elem->~Element();
                }
                release<Element>(*the_tail.page);
                the_tail.page--;
            }
            release<Element*>(the_head);
            init();
        }
    }
    template<class Iterator>
    void erase_by_iterator(Iterator it)
    {
        assert(the_page_size);
        if(it.next())
        {
            Cursor cr, to, from;
            assert(it.value() < size());
            convert(to, it.value());
            to.elem->~Element();
            from = to;
            while(it.next())
            {
                assert(it.value() < size());
                convert(cr, it.value());
                cr.elem->~Element();
                move_down(to, from, cr);
            }
            move_down(to, from, the_tail);
            release_tail(to);
        }
    }
    Element& at(uint32 pos)
    {
        assert(the_page_size && pos < size());
        return the_head[pos / the_page_size][pos % the_page_size];
    }
    Element& put(uint32 pos, const Element& arg)
    {
        assert(the_page_size);
        acquire_tail();
        assert(pos < size());
        Cursor cr;
        convert(cr, pos);
        move_up(cr);
        new((void*)cr.elem) Element(arg);
        return *cr.elem;
    }
    void erase(uint32 pos)
    {
        assert(the_page_size && pos < size());
        Cursor to, from;
        convert(to, pos);
        to.elem->~Element();
        from = to;
        move_down(to, from, the_tail);
        release_tail();
    }
    void erase_by_tail(uint32 pos)
    {
        assert(the_page_size && pos < size());
        Element* elem = the_head[pos / the_page_size] + (pos % the_page_size);
        elem->~Element();
        ::memcpy((void*)elem, the_tail.elem - 1, sizeof(Element));
        release_tail();
    }
    Element& append()
    {
        assert(the_page_size);
        Element* elem = acquire_tail();
        new((void*)elem) Element();
        return *elem;
    }
    Element& append(const Element& arg)
    {
        assert(the_page_size);
        Element* elem = acquire_tail();
        new((void*)elem) Element(arg);
        return *elem;
    }
    uint32 search(const Element& arg)
    {
        assert(the_page_size);
        int32 lbond = 0;
        int32 hbond = size() - 1;
        while(lbond <= hbond)
        {
            uint32 pos = (lbond + hbond) >> 1;
            int32 dir = the_index.compare((Element&)arg, at(pos));
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
                return pos;
        }
        return uint32_nil;
    }
    Shared<Element> lookup(const Element& arg)
    {
        assert(the_page_size);
        int32 lbond = 0;
        int32 hbond = size() - 1;
        while(lbond <= hbond)
        {
            uint32 pos = (lbond + hbond) >> 1;
            Element& result = at(pos);
            int32 dir = the_index.compare((Element&)arg, result);
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
        assert(the_page_size);
        int32 lbond = 0;
        int32 hbond = size() - 1;
        while(lbond <= hbond)
        {
            uint32 pos = (lbond + hbond) >> 1;
            Element& result = at(pos);
            int32 dir = the_index.compare((Element&)arg, result);
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else
                return result;
        }
        acquire_tail();
        Cursor cr;
        convert(cr, lbond);
        move_up(cr);
        new((void*)cr.elem) Element(pager.append());
        return *cr.elem;
    }
    bool put(const Element& arg, bool unique = true)
    {
        assert(the_page_size);
        int32 lbond = 0;
        int32 hbond = size() - 1;
        while(lbond <= hbond)
        {
            uint32 pos = (lbond + hbond) >> 1;
            int32 dir = the_index.compare((Element&)arg, at(pos));
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
        Cursor cr;
        convert(cr, lbond);
        move_up(cr);
        new((void*)cr.elem) Element(arg);
        return true;
    }
    uint32 erase(const Element& arg, bool unique = true)
    {
        assert(the_page_size);
        int32 lbond = 0;
        int32 hbond = size() - 1;
        while(lbond <= hbond)
        {
            uint32 pos = (lbond + hbond) >> 1;
            int32 dir = the_index.compare((Element&)arg, at(pos));
            if(0 > dir)
                hbond = pos - 1;
            else if(0 < dir)
                lbond = pos + 1;
            else if(!unique)
            {
                Cursor to, from;
                convert(to, pos);
                from = to;
                while((to.elem != *the_head) && !the_index.compare((Element&)arg, *to.elem))
                    prev(to);
                while((from.elem != the_tail.elem - 1) && !the_index.compare((Element&)arg, *from.elem))
                    next(from);
                if(the_index.compare((Element&)arg, *to.elem))
                    next(to);
                if(the_index.compare((Element&)arg, *from.elem))
                    prev(from);
                uint32 erased = distance(from, to) + 1;
                Cursor it = to;
                while(it.elem != from.elem)
                {
                    it.elem->~Element();
                    next(it);
                }
                from.elem->~Element();
                move_down(to, from, the_tail);
                release_tail(to);
                return erased;
            }
            else
            {
                Cursor to, from;
                convert(to, pos);
                to.elem->~Element();
                from = to;
                move_down(to, from, the_tail);
                release_tail();
                return 1;
            }
        }
        return 0;
    }
private:
    struct Cursor
    {
        Element** page;
        Element* elem;
    };
private:
    Element* acquire_tail()
    {
        if(the_tail.elem == the_bond)
        {
            if(the_bond)
            {
                uint32 tail_page = the_tail.page - the_head + 1;
                the_head = core::acquire<Element*>(the_head, sizeof(Element*) * (tail_page + 1));
                the_tail.page = the_head + tail_page;
            }
            else
            {
                the_head = core::acquire<Element*>(sizeof(Element*));
                the_tail.page = the_head;
            }
            *the_tail.page = core::acquire<Element>(sizeof(Element) * the_page_size);
            the_tail.elem = *the_tail.page;
            the_bond = *the_tail.page + the_page_size;
        }
        return the_tail.elem++;
    }
    void release_tail()
    {
        the_tail.elem--;
        if(the_tail.elem == the_bond - the_page_size)
        {
            release<Element>(*the_tail.page);
            the_tail.page--;
            if(the_tail.page >= the_head)
            {
                the_tail.elem = *the_tail.page + the_page_size;
                the_bond = the_tail.elem;
            }
            else
            {
                release<Element*>(the_head);
                init();
            }
        }
    }
    void release_tail(Cursor& to)
    {
        if(to.elem == *to.page)
        {
            to.page--;
            if(to.page >= the_head)
                to.elem = *to.page + the_page_size;
        }
        while(the_tail.page > to.page)
        {
            release<Element>(*the_tail.page);
            the_tail.page--;
        }
        if(to.page >= the_head)
        {
            the_tail.elem = to.elem;
            the_bond = *to.page + the_page_size;
        }
        else
        {
            release<Element*>(the_head);
            init();
        }
    }
    void convert(Cursor& cr, uint32 pos)
    {
        cr.page = the_head + (pos / the_page_size);
        cr.elem = *cr.page + (pos % the_page_size);
    }
    void next(Cursor& cr)
    {
        cr.elem++;
        if(cr.elem == *cr.page + the_page_size)
        {
            cr.page++;
            cr.elem = *cr.page;
        }
    }
    void prev(Cursor& cr)
    {
        if(cr.elem == *cr.page)
        {
            cr.page--;
            cr.elem = *cr.page + the_page_size;
        }
        cr.elem--;
    }
    uint32 distance(Cursor& high, Cursor& low)
    {
        return the_page_size * (high.page - low.page) + (high.elem - *high.page) - (low.elem - *low.page);
    }
    void move_up(Cursor& bond)
    {
        Cursor to, from;
        to = from = the_tail;
        prev(from);
        while(from.elem != bond.elem)
        {
            if(from.elem == *from.page)
            {
                from.page--;
                from.elem = *from.page + the_page_size;
            }
            if(to.elem == *to.page)
            {
                to.page--;
                to.elem = *to.page + the_page_size;
            }
            uint32 sh = (from.page > bond.page)
                ? min(from.elem - *from.page, to.elem - *to.page)
                : min(from.elem - bond.elem, to.elem - *to.page);
            from.elem -= sh;
            to.elem -= sh;
            ::memmove((void*)to.elem, from.elem, sizeof(Element) * sh);
        }
    }
    void move_down(Cursor& to, Cursor& from, Cursor& bond)
    {
        from.elem++;
        if((from.elem == *from.page + the_page_size) && (from.elem != bond.elem))
        {
            from.page++;
            from.elem = *from.page;
        }
        while(from.elem != bond.elem)
        {
            uint32 sh = (from.page < bond.page)
                ? the_page_size - max(to.elem - *to.page, from.elem - *from.page)
                : min(*to.page + the_page_size - to.elem, bond.elem - from.elem);
            ::memmove((void*)to.elem, from.elem, sizeof(Element) * sh);
            to.elem += sh;
            if(to.elem == *to.page + the_page_size)
            {
                to.page++;
                to.elem = *to.page;
            }
            from.elem += sh;
            if((from.elem == *from.page + the_page_size) && (from.elem != bond.elem))
            {
                from.page++;
                from.elem = *from.page;
            }
        }
    }
    void init()
    {
        the_bond = the_tail.elem = 0;
        the_tail.page = the_head = &the_bond;
    }
private:
    Element** the_head;
    Element* the_bond;
    Cursor the_tail;
    uint32 the_page_size;
    ListIndex the_index;
};
