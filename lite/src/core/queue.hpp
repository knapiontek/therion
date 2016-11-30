
template<class Element>
class Queue : Iterable
{
public:
    typedef Shared<Queue> share;
    typedef Managed<Queue> manage;
public:
    Queue()
    {
        the_page_size = 0;
    }
    Queue(int64 page_size)
    {
        init(page_size);
    }
    ~Queue()
    {
        if(the_page_size)
            erase_all(true);
    }
    void page_size(int64 page_size)
    {
        certify(!the_page_size);
        init(page_size);
    }
    int64 page_size()
    {
        return the_page_size;
    }
    int64 size()
    {
        certify(the_page_size);
        auto size = the_tail - the_tail_page->data;
        auto page = the_head_page;
        while(page->next)
        {
            size += the_page_size;
            page = page->next;
        }
        size -= the_head - the_head_page->data;
        return size;
    }
    bool is_empty()
    {
        certify(the_page_size);
        return the_tail == the_head;
    }
    void erase_all()
    {
        certify(the_page_size);
        erase_all(false);
    }
    void push(const Element& element)
    {
        certify(the_page_size);
        if(the_tail == the_tail_page->data + the_page_size)
        {
            the_tail_page->next = acquire<Page>(sizeof(Page) + sizeof(Element) * (the_page_size - 1));
            the_tail_page = the_tail_page->next;
            the_tail_page->next = 0;
            the_tail = the_tail_page->data;
        }
        new((void*)the_tail) Element(element);
        the_tail++;
    }
    bool pop(Element& element)
    {
        certify(the_page_size);
        if(the_head != the_tail)
        {
            element = *the_head;
            the_head->~Element();
            the_head++;
            if(the_head == the_tail)
                the_head = the_tail = the_head_page->data;
            else if(the_head == the_head_page->data + the_page_size)
            {
                auto page = the_head_page;
                the_head_page = the_head_page->next;
                the_head = the_head_page->data;
                release<Page>(page);
            }
            return true;
        }
        return false;
    }
    bool pop()
    {
        certify(the_page_size);
        if(the_head != the_tail)
        {
            the_head->~Element();
            the_head++;
            if(the_head == the_tail)
                the_head = the_tail = the_head_page->data;
            else if(the_head == the_head_page->data + the_page_size)
            {
                auto page = the_head_page;
                the_head_page = the_head_page->next;
                the_head = the_head_page->data;
                release<Page>(page);
            }
            return true;
        }
        return false;
    }
    bool peek(Element& element)
    {
        certify(the_page_size);
        if(the_head != the_tail)
        {
            element = *the_head;
            return true;
        }
        return false;
    }
    Shared<Element> peek()
    {
        certify(the_page_size);
        if(the_head != the_tail)
            return Shared<Element>(*the_head);
        return Shared<Element>();
    }
private:
    struct Page
    {
        Page* next;
        Element data[1];
    };
private:
    void init(int64 page_size)
    {
        certify(page_size);
        the_page_size = page_size;
        the_tail_page = acquire<Page>(sizeof(Page) + sizeof(Element) * (the_page_size - 1));
        the_tail_page->next = 0;
        the_head_page = the_tail_page;
        the_head = the_tail = the_tail_page->data;
    }
    void erase_all(bool final)
    {
        auto page = the_head_page;
        while(page)
        {
            auto pos = (page != the_head_page)
                ? page->data
                : the_head;
            auto tail = (page != the_tail_page)
                ? page->data + the_page_size
                : the_tail;
            while(pos < tail)
            {
                pos->~Element();
                pos++;
            }
            if(final || page != the_tail_page)
            {
                auto free_page = page;
                page = page->next;
                release<Page>(free_page);
            }
            else
            {
                the_tail_page->next = 0;
                the_head_page = the_tail_page;
                the_head = the_tail = the_tail_page->data;
                return;
            }
        }
    }
private:
    Page* the_tail_page;
    Page* the_head_page;
    Element* the_tail;
    Element* the_head;
    int64 the_page_size;
};
