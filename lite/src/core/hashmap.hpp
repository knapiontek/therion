
template<class Key, class Value, class HashIndex = Index<Key>>
class HashMap : Iterable
{
    struct Node;
    struct Page;
public:
    typedef Shared<HashMap> share;
    typedef Managed<HashMap> manage;
public:
    class Iterator
    {
        friend class HashMap;
    public:
        bool is_head()
        {
            return !the_page->prev && the_node == the_page->data;
        }
        bool is_tail()
        {
            return the_node == the_map->the_tail;
        }
        bool next()
        {
            the_node++;
            if(the_node == the_bond)
            {
                if(!the_page->next)
                    return false;
                the_page = the_page->next;
                the_node = the_page->data;
                the_bond = the_page->next
                    ? the_page->data + the_map->the_page_size
                    : the_map->the_tail;
            }
            return true;
        }
        bool prev()
        {
            if(the_node == the_page->data)
            {
                if(!the_page->prev)
                    return false;
                the_page = the_page->prev;
                the_node = the_page->data + the_map->the_page_size;
                the_bond = the_node;
            }
            the_node--;
            return true;
        }
        Key& key()
        {
            return the_node->key;
        }
        Value& value()
        {
            return the_node->value;
        }
        bool operator!=(Iterator& it)
        {
            return the_node != it.the_node;
        }
        void operator++()
        {
            the_node++;
            if(the_node == the_bond && the_page->next)
            {
                the_page = the_page->next;
                the_node = the_page->data;
                the_bond = the_page->next
                    ? the_page->data + the_map->the_page_size
                    : the_map->the_tail;
            }
        }
        void operator--()
        {
            if(the_node == the_page->data && the_page->prev)
            {
                the_page = the_page->prev;
                the_node = the_page->data + the_map->the_page_size;
                the_bond = the_node;
            }
            the_node--;
        }
        Iterator& operator*()
        {
            return *this;
        }
    private:
        Iterator(Page* page, Node* node, Node* bond, HashMap* map)
        {
            the_page = page;
            the_node = node;
            the_bond = bond;
            the_map = map;
        }
    private:
        Page* the_page;
        Node* the_node;
        Node* the_bond;
        HashMap* the_map;
    };
    class Reverse : public Iterator
    {
        friend class HashMap;
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
        friend class HashMap;
    public:
        bool next()
        {
            the_node = the_node->next;
            while(the_node)
            {
                if(!the_map->the_index.compare(const_cast<Key&>(the_key), the_node->key))
                    return true;
                the_node = the_node->next;
            }
            return false;
        }
        Key& key()
        {
            return the_node->key;
        }
        Value& value()
        {
            return the_node->value;
        }
        friend bool operator==(Nil, const Find& arg)
        {
            return !arg.the_node->next;
        }
        friend bool operator!=(Nil, const Find& arg)
        {
            return arg.the_node->next;
        }
        friend bool operator==(const Find& arg, Nil)
        {
            return !arg.the_node->next;
        }
        friend bool operator!=(const Find& arg, Nil)
        {
            return arg.the_node->next;
        }
    private:
        Find(Node* pos, Key& key, HashMap* map) : the_key(key)
        {
            the_node = &the_sentinel;
            the_node->next = pos;
            the_map = map;
        }
    private:
        Node the_sentinel;
        Node* the_node;
        Key& the_key;
        HashMap* the_map;
    };
public:
    HashMap()
    {
        the_page_size = 0;
    }
    HashMap(int64 page_size)
    {
        init(page_size);
    }
    HashMap(const std::initializer_list<std::pair<Key, Value>>& list)
    {
        init(list.size());

        for(auto& i : list)
        {
            put(i.first, i.second);
        }
    }
    ~HashMap()
    {
        if(the_page_size)
            erase_all(true);
    }
    Iterator head()
    {
        certify(the_page_size);
        auto bond = (the_head_page != the_tail_page)
            ? the_head_page->data + the_page_size
            : the_tail;
        return Iterator(the_head_page, the_head_page->data - 1, bond, this);
    }
    Iterator tail()
    {
        certify(the_page_size);
        return Iterator(the_tail_page, the_tail, the_tail, this);
    }
    Iterator begin()
    {
        certify(the_page_size);
        auto bond = (the_head_page != the_tail_page)
            ? the_head_page->data + the_page_size
            : the_tail;
        return Iterator(the_head_page, the_head_page->data, bond, this);
    }
    Iterator end()
    {
        return tail();
    }
    Reverse rbegin()
    {
        certify(the_page_size);
        return Iterator(the_tail_page, the_tail - 1, the_tail, this);
    }
    Reverse rend()
    {
        return head();
    }
    Find find(const Key& key)
    {
        certify(the_page_size);
        auto pos = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        return Find(pos, const_cast<Key&>(key), this);
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
        return size;
    }
    bool is_empty()
    {
        certify(the_page_size);
        return the_tail == the_head_page->data;
    }
    void erase_all()
    {
        certify(the_page_size);
        erase_all(false);
    }
    Shared<Value> lookup(const Key& key)
    {
        certify(the_page_size);
        auto pos = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        while(pos)
        {
            if(!the_index.compare(const_cast<Key&>(key), pos->key))
                return pos->value;
            pos = pos->next;
        }
        return nil;
    }
    bool set(const Key& key, const Value& value)
    {
        certify(the_page_size);
        auto pos = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        while(pos)
        {
            if(!the_index.compare(const_cast<Key&>(key), pos->key))
            {
                pos->value = value;
                return true;
            }
            pos = pos->next;
        }
        return false;
    }
    Value& at(const Key& key)
    {
        certify(the_page_size);
        auto& head = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        // search
        auto pos = head;
        while(pos)
        {
            if(!the_index.compare(const_cast<Key&>(key), pos->key))
                return pos->value;
            pos = pos->next;
        }
        // create new head
        pos = head;
        head = acquire_tail();
        head->next = pos;
        new((void*)&head->key) Key(key);
        new((void*)&head->value) Value();
        return head->value;
    }
    Value& operator[](const Key& key)
    {
        return at(key);
    }
    Value& at(const Key& key, const Value& value)
    {
        certify(the_page_size);
        auto& head = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        // search
        auto pos = head;
        while(pos)
        {
            if(!the_index.compare(const_cast<Key&>(key), pos->key))
                return pos->value;
            pos = pos->next;
        }
        // create new head
        pos = head;
        head = acquire_tail();
        head->next = pos;
        new((void*)&head->key) Key(key);
        new((void*)&head->value) Value(value);
        return head->value;
    }
    template<class Pager>
    Value& acquire(const Key& key, Pager& pager)
    {
        certify(the_page_size);
        auto& head = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        // search
        auto pos = head;
        while(pos)
        {
            if(!the_index.compare(const_cast<Key&>(key), pos->key))
                return pos->value;
            pos = pos->next;
        }
        // create new head
        pos = head;
        head = acquire_tail();
        head->next = pos;
        new((void*)&head->key) Key(key);
        new((void*)&head->value) Value(pager.append());
        return head->value;
    }
    bool put(const Key& key, const Value& value, bool unique = true)
    {
        certify(the_page_size);
        auto& head = the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        // search
        if(unique)
        {
            auto pos = head;
            while(pos)
            {
                if(!the_index.compare(const_cast<Key&>(key), pos->key))
                    return false;
                pos = pos->next;
            }
        }
        // create new head
        auto pos = head;
        head = acquire_tail();
        head->next = pos;
        new((void*)&head->key) Key(key);
        new((void*)&head->value) Value(value);
        return true;
    }
    int64 erase(const Key& key, bool unique = true)
    {
        certify(the_page_size);
        auto erased = 0;
        auto prev = &the_plexer[the_index.hash(const_cast<Key&>(key)) % the_page_size];
        auto pos = *prev;
        while(pos)
        {
            if(!the_index.compare(const_cast<Key&>(key), pos->key))
            {
                // remove matching tail
                auto tail = the_tail - 1;
                while(pos != tail && !the_index.compare(const_cast<Key&>(key), tail->key))
                {
                    // remove tail from linked list
                    auto tail_prev = &the_plexer[the_index.hash(tail->key) % the_page_size];
                    auto tail_pos = *tail_prev;
                    while(tail_pos != tail)
                    {
                        tail_prev = &tail_pos->next;
                        tail_pos = *tail_prev;
                    }
                    *tail_prev = tail->next;
                    tail->key.~Key();
                    tail->value.~Value();
                    release_tail();
                    if(unique) return 1;
                    erased++;
                    tail = the_tail - 1;
                }
                // remove matching pos
                *prev = pos->next;
                pos->key.~Key();
                pos->value.~Value();
                if(pos != tail)
                {
                    // update tail/pos linked list
                    auto tail_prev = &the_plexer[the_index.hash(tail->key) % the_page_size];
                    auto tail_pos = *tail_prev;
                    while(tail_pos != tail)
                    {
                        tail_prev = &tail_pos->next;
                        tail_pos = *tail_prev;
                    }
                    *tail_prev = pos;
                    // override pos with tail
                    ::memcpy(pos, tail, sizeof(Node));
                }
                release_tail(); // or and pos
                if(unique) return 1;
                erased++;
                if(prev == &tail->next)
                    prev = &pos->next; // see memcpy
                pos = *prev;
            }
            else
            {
                prev = &pos->next;
                pos = *prev;
            }
        }
        return erased;
    }
private:
    struct Node
    {
        Node* next;
        Key key;
        Value value;
    };
    struct Page
    {
        Page* next;
        Page* prev;
        Node data[1];
    };
private:
    void init(int64 page_size)
    {
        certify(page_size);
        the_page_size = page_size + 1;
        auto mem_page_size = sizeof(Page) + sizeof(Node) * (the_page_size - 1);
        auto mem_plexer_size = sizeof(Node*) * the_page_size;
        auto mem = core::acquire<uint8>(mem_page_size + mem_plexer_size);
        the_head_page = reinterpret_cast<Page*>(mem);
        the_head_page->next = 0;
        the_head_page->prev = 0;
        the_tail_page = the_head_page;
        the_tail = the_tail_page->data;
        the_plexer = reinterpret_cast<Node**>(mem + mem_page_size);
        ::bzero(the_plexer, mem_plexer_size);
    }
    void erase_all(bool final)
    {
        while(the_tail_page)
        {
            while(the_tail > the_tail_page->data)
            {
                the_tail--;
                the_tail->key.~Key();
                the_tail->value.~Value();
            }
            if(final || the_tail_page != the_head_page)
            {
                auto page = the_tail_page;
                the_tail_page = the_tail_page->prev;
                the_tail = the_tail_page->data + the_page_size; // 0x0->data is 0x8 and it is NOT a bug
                release<Page>(page);
            }
            else
            {
                the_tail_page->next = 0;
                ::bzero(the_plexer, sizeof(Node*) * the_page_size);
                return;
            }
        }
    }
    Node* acquire_tail()
    {
        if(the_tail == the_tail_page->data + the_page_size)
        {
            auto page = the_tail_page;
            the_tail_page = core::acquire<Page>(sizeof(Page) + sizeof(Node) * (the_page_size - 1));
            the_tail_page->next = 0;
            the_tail_page->prev = page;
            page->next = the_tail_page;
            the_tail = the_tail_page->data;
        }
        return the_tail++;
    }
    void release_tail()
    {
        the_tail--;
        if(the_tail == the_tail_page->data && the_tail_page != the_head_page)
        {
            auto page = the_tail_page;
            the_tail_page = the_tail_page->prev;
            the_tail_page->next = 0;
            the_tail = the_tail_page->data + the_page_size;
            release<Page>(page);
        }
    }
private:
    Page* the_head_page;
    Page* the_tail_page;
    Node* the_tail;
    Node** the_plexer;
    int64 the_page_size;
    HashIndex the_index;
};
