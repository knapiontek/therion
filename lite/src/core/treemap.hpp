
const auto the_treemap_path_size = 32;

template< typename Key, typename Value, class TreeIndex = Index<Key> >
class TreeMap
{
    struct Node;
    struct Page;
public:
    typedef Shared<TreeMap> share;
    typedef Managed<TreeMap> manage;
public:
    class Iterator
    {
        friend class TreeMap;
    public:
        bool is_head()
        {
            return the_page == the_map->the_head_page && the_node == the_page->data;
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
                if(the_node == the_map->the_tail)
                    return false;
                the_page = the_page->next;
                the_node = the_page->data;
                the_bond = (the_page != the_map->the_tail_page)
                    ? the_page->data + the_map->the_page_size
                    : the_map->the_tail;
            }
            return true;
        }
        bool prev()
        {
            if(the_node == the_page->data)
            {
                if(the_page == the_map->the_head_page)
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
    private:
        Iterator(Page* page, Node* node, Node* bond, TreeMap* map)
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
        TreeMap* the_map;
    };
    class Sort
    {
        friend class TreeMap;
    public:
        Sort& operator=(const Sort& it)
        {
            the_sentinel[0] = it.the_sentinel[0];
            the_sentinel[1] = it.the_sentinel[1];
            the_node = the_path;
            *the_node = (Node*)the_sentinel;
            return *this;
        }
        bool next()
        {
            Node* pos = (*the_node)->leaves[1];
            the_node--;
            while(pos)
            {
                *++the_node = pos;
                pos = pos->leaves[0];
            }
            return the_node >= the_path;
        }
        bool prev()
        {
            Node* pos = (*the_node)->leaves[0];
            the_node--;
            while(pos)
            {
                *++the_node = pos;
                pos = pos->leaves[1];
            }
            return the_node >= the_path;
        }
        Key& key()
        {
            return (*the_node)->key;
        }
        Value& value()
        {
            return (*the_node)->value;
        }
    private:
        Sort(Node* root)
        {
            the_sentinel[0] = root;
            the_sentinel[1] = root;
            the_node = the_path;
            *the_node = (Node*)the_sentinel;
        }
    private:
        Node* the_path[the_treemap_path_size];
        Node* the_sentinel[2];
        Node** the_node;
    };
    class Find
    {
        friend class TreeMap;
    public:
        bool next()
        {
            Node* pos;
            switch(the_state)
            {
            case none:
                return false;
            case left: // set on the left of the root
                pos = (*the_node)->leaves[0];
                the_node--;
                while(pos)
                {
                    *++the_node = pos;
                    pos = pos->leaves[1];
                }
                if(the_node < the_path || the_map->the_index.compare((Key&)*the_key, (*the_node)->key)) // set on the root
                {
                    the_node = the_path;
                    *the_node = the_root;
                    the_state = right;
                }
                return true;
            case right:
                pos = (*the_node)->leaves[1];
                the_node--;
                while(pos)
                {
                    *++the_node = pos;
                    pos = pos->leaves[0];
                }
                return the_node >= the_path && !the_map->the_index.compare((Key&)*the_key, (*the_node)->key);
            }
            return false; // compiler cannot see that all states end by return statement
        }
        Key& key()
        {
            return (*the_node)->key;
        }
        Value& value()
        {
            return (*the_node)->value;
        }
        friend bool operator==(const Nil&, const Find& arg)
        {
            return none == arg.the_state;
        }
        friend bool operator!=(const Nil&, const Find& arg)
        {
            return none != arg.the_state;
        }
        friend bool operator==(const Find& arg, const Nil&)
        {
            return none == arg.the_state;
        }
        friend bool operator!=(const Find& arg, const Nil&)
        {
            return none != arg.the_state;
        }
    private:
        Find()
        {
            the_state = none;
        }
        Find(Node* root, Key* key, TreeMap* map)
        {
            the_root = root;
            the_node = the_path;
            *the_node = the_root;
            the_key = key;
            the_state = left;
            the_map = map;
        }
    private:
        enum { none, left, right} the_state;
        Node* the_path[the_treemap_path_size];
        Node** the_node;
        Node* the_root;
        Key* the_key;
        TreeMap* the_map;
    };
public:
    TreeMap()
    {
        the_page_size = 0;
    }
    TreeMap(int64 page_size)
    {
        init(page_size);
    }
    ~TreeMap()
    {
        if(the_page_size)
            erase_all(true);
    }
    Iterator head()
    {
        assert(the_page_size);
        Node* bond = (the_head_page != the_tail_page)
            ? the_head_page->data + the_page_size
            : the_tail;
        return Iterator(the_head_page, the_head_page->data - 1, bond, this);
    }
    Iterator tail()
    {
        assert(the_page_size);
        return Iterator(the_tail_page, the_tail, the_tail, this);
    }
    Sort sort()
    {
        assert(the_page_size);
        return Sort(the_root);
    }
    Find find(const Key& key)
    {
        assert(the_page_size);
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
                pos = pos->leaves[0 < dir];
            else
                return Find(pos, (Key*)&key, this);
        }
        return Find();
    }
    void page_size(int64 page_size)
    {
        assert(!the_page_size);
        init(page_size);
    }
    int64 page_size()
    {
        return the_page_size;
    }
    int64 size()
    {
        assert(the_page_size);
        auto size = the_tail - the_tail_page->data;
        Page* page = the_head_page;
        while(page->next)
        {
            size += the_page_size;
            page = page->next;
        }
        return size;
    }
    bool is_empty()
    {
        assert(the_page_size);
        return the_tail == the_head_page->data;
    }
    void erase_all()
    {
        assert(the_page_size);
        erase_all(false);
    }
    Shared<Value> lookup(const Key& key)
    {
        assert(the_page_size);
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
                pos = pos->leaves[0 < dir];
            else
                return pos->value;
        }
        return nil;
    }
    bool set(const Key& key, const Value& value)
    {
        assert(the_page_size);
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
                pos = pos->leaves[0 < dir];
            else
            {
                pos->value = value;
                return true;
            }
        }
        return false;
    }
    Value& at(const Key& key)
    {
        assert(the_page_size);

        // add the_root to path/used as i->node->leaves[i->dir] = root;
        Path path[the_treemap_path_size];
        Path* i = path;
        i->node = (Node*)&the_root;
        i->dir = 0;

        // search/build path
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
            {
                i++;
                i->node = pos;
                i->dir = 0 < dir;
                pos = pos->leaves[i->dir];
            }
            else
                return pos->value;
        }

        // create node
        pos = i->node->leaves[i->dir] = acquire_tail();
        pos->leaves[0] = pos->leaves[1] = 0;
        pos->balance = 0;
        new((void*)&pos->key) Key(key);
        new((void*)&pos->value) Value();

        // rebalance
        insert_balance(i, path);
        assert(-1 != self_test(the_root));
        return pos->value;
    }
    Value& at(const Key& key, const Value& value)
    {
        assert(the_page_size);

        // add the_root to path/used as i->node->leaves[i->dir] = root;
        Path path[the_treemap_path_size];
        Path* i = path;
        i->node = (Node*)&the_root;
        i->dir = 0;

        // search/build path
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
            {
                i++;
                i->node = pos;
                i->dir = 0 < dir;
                pos = pos->leaves[i->dir];
            }
            else
                return pos->value;
        }

        // create node
        pos = i->node->leaves[i->dir] = acquire_tail();
        pos->leaves[0] = pos->leaves[1] = 0;
        pos->balance = 0;
        new((void*)&pos->key) Key(key);
        new((void*)&pos->value) Value(value);

        // rebalance
        insert_balance(i, path);
        assert(-1 != self_test(the_root));
        return pos->value;
    }
    template<class Pager>
    Value& acquire(const Key& key, Pager& pager)
    {
        assert(the_page_size);

        // add the_root to path/used as i->node->leaves[i->dir] = root;
        Path path[the_treemap_path_size];
        Path* i = path;
        i->node = (Node*)&the_root;
        i->dir = 0;

        // search/build path
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
            {
                i++;
                i->node = pos;
                i->dir = 0 < dir;
                pos = pos->leaves[i->dir];
            }
            else
                return pos->value;
        }

        // create node
        pos = i->node->leaves[i->dir] = acquire_tail();
        pos->leaves[0] = pos->leaves[1] = 0;
        pos->balance = 0;
        new((void*)&pos->key) Key(key);
        new((void*)&pos->value) Value(pager.append());

        // rebalance
        insert_balance(i, path);
        assert(-1 != self_test(the_root));
        return pos->value;
    }
    bool put(const Key& key, const Value& value, bool unique = true)
    {
        assert(the_page_size);

        // add the_root to path/used as i->node->leaves[i->dir] = root;
        Path path[the_treemap_path_size];
        Path* i = path;
        i->node = (Node*)&the_root;
        i->dir = 0;

        // search/build path
        Node* pos = the_root;
        while(pos)
        {
            auto dir = the_index.compare((Key&)key, pos->key);
            if(dir)
            {
                i++;
                i->node = pos;
                i->dir = 0 < dir;
                pos = pos->leaves[i->dir];
            }
            else if(!unique)
            {
                i++;
                i->node = pos;
                i->dir = 0;
                pos = pos->leaves[0];
                while(pos)
                {
                    i++;
                    i->node = pos;
                    i->dir = 1;
                    pos = pos->leaves[1];
                }
                break;
            }
            else
                return false;
        }

        // create node
        pos = i->node->leaves[i->dir] = acquire_tail();
        pos->leaves[0] = pos->leaves[1] = 0;
        pos->balance = 0;
        new((void*)&pos->key) Key(key);
        new((void*)&pos->value) Value(value);

        // rebalance
        insert_balance(i, path);
        assert(-1 != self_test(the_root));
        return true;
    }
    int64 erase(const Key& key, bool unique = true)
    {
        assert(the_page_size);
        int64 cnt;
        auto erased = 0;
        do
        {
            cnt = erased;
            Path path[the_treemap_path_size];
            Path* i = path;
            i->node = (Node*)&the_root;
            i->dir = 0;

            // search key
            Node* pos = the_root;
            while(pos)
            {
                auto dir = the_index.compare((Key&)key, pos->key);
                if(dir)
                {
                    i++;
                    i->node = pos;
                    i->dir = 0 < dir;
                    pos = pos->leaves[i->dir];
                }
                else
                {
                    // remove key and replace with the greatest lesser
                    pos->key.~Key();
                    pos->value.~Value();
                    Node* lesser = pos->leaves[0];
                    if(lesser)
                    {
                        if(lesser->leaves[1])
                        {
                            Path* parent = i;
                            Path* holder = ++i; // holder for pos substitution
                            do
                            {
                                i++;
                                i->node = lesser;
                                i->dir = 1;
                                lesser = lesser->leaves[1];
                            } while(lesser);
                            Node* detached = i->node->leaves[0];

                            // replace pos with bottom leaf
                            parent->node->leaves[parent->dir] = i->node;
                            holder->node = i->node;
                            holder->dir = 0;

                            i->node->leaves[0] = pos->leaves[0];
                            i->node->leaves[1] = pos->leaves[1];
                            i->node->balance = pos->balance;

                            i--;
                            i->node->leaves[i->dir] = detached;
                        }
                        else
                        {
                            i->node->leaves[i->dir] = lesser; // replace pos with left leaf (lesser)
                            lesser->leaves[1] = pos->leaves[1];
                            lesser->balance = pos->balance;
                            i++;
                            i->node = lesser;
                            i->dir = 0;
                        }
                    }
                    else
                        i->node->leaves[i->dir] = pos->leaves[1];

                    // rebalance
                    erase_balance(i, path);
                    assert(-1 != self_test(the_root));

                    // relocate tail to fill in a gap
                    Node* tail = the_tail - 1;
                    if(pos != tail)
                    {
                        Node* heap[the_treemap_path_size];
                        Node** hp = heap;
                        Node** parent = &the_root;
                        Node* tail_pos = the_root;
                        while(tail_pos != tail)
                        {
                            auto dir = the_index.compare(tail->key, tail_pos->key);
                            if(dir)
                            {
                                parent = &tail_pos->leaves[0 < dir];
                                tail_pos = *parent;
                            }
                            else
                            {
                                *(hp++) = tail_pos;
                                parent = &tail_pos->leaves[0];
                                tail_pos = *parent;
                            }
                            while(!tail_pos)
                            {
                                tail_pos = *(--hp);
                                assert(hp >= heap);
                                parent = &tail_pos->leaves[1];
                                tail_pos = *parent;
                            }
                        }
                        *parent = pos;
                        ::memcpy(pos, tail, sizeof(Node));
                    }
                    release_tail();
                    erased++;
                    if(unique) return 1;
                    else break;
                }
            }
        } while(cnt != erased);
        return erased;
    }
private:
    struct Node
    {
        Node* leaves[2]; // must be first
        int8 balance;
        Key key;
        Value value;
    };
    struct Page
    {
        Page* next;
        Page* prev;
        Node data[1];
    };
    struct Path
    {
        Node* node;
        bool dir;
    };
private:
    void erase_balance(Path* i, Path* path)
    {
        while(i > path)
        {
            const int8 context[] = { 1, -1 };
            i->node->balance += context[i->dir];
            switch(i->node->balance)
            {
            case 0:
                break;
            case -1:
            case 1:
                return;
            case -2:
                if(1 == i->node->leaves[0]->balance)
                {
                    Node* middle = i->node->leaves[0];
                    Node* root = middle->leaves[1];
                    i->node->leaves[0] = root->leaves[1];
                    middle->leaves[1] = root->leaves[0];
                    root->leaves[1] = i->node;
                    root->leaves[0] = middle;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = !(1 + root->balance);
                    middle->balance = -!(1 - root->balance);
                    root->balance = 0;
                    break;
                }
                else
                {
                    Node* root = i->node->leaves[0];
                    i->node->leaves[0] = root->leaves[1];
                    root->leaves[1] = i->node;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = -!root->balance;
                    root->balance = !root->balance;
                    if(root->balance) return;
                    else break;
                }
            case 2:
                if(-1 == i->node->leaves[1]->balance)
                {
                    Node* middle = i->node->leaves[1];
                    Node* root = middle->leaves[0];
                    i->node->leaves[1] = root->leaves[0];
                    middle->leaves[0] = root->leaves[1];
                    root->leaves[0] = i->node;
                    root->leaves[1] = middle;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = -!(1 - root->balance);
                    middle->balance = !(1 + root->balance);
                    root->balance = 0;
                    break;
                }
                else
                {
                    Node* root = i->node->leaves[1];
                    i->node->leaves[1] = root->leaves[0];
                    root->leaves[0] = i->node;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = !root->balance;
                    root->balance = -!root->balance;
                    if(root->balance) return;
                    else break;
                }
            }
            i--;
        }
    }
    void insert_balance(Path* i, Path* path)
    {
        while(i > path)
        {
            const int8 context[] = { -1, 1 };
            i->node->balance += context[i->dir];
            switch(i->node->balance)
            {
            case 0:
                return;
            case -1:
            case 1:
                i--;
                break;
            case -2:
                if(1 == i->node->leaves[0]->balance)
                {
                    Node* middle = i->node->leaves[0];
                    Node* root = middle->leaves[1];
                    i->node->leaves[0] = root->leaves[1];
                    middle->leaves[1] = root->leaves[0];
                    root->leaves[1] = i->node;
                    root->leaves[0] = middle;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = !(1 + root->balance);
                    middle->balance = -!(1 - root->balance);
                    root->balance = 0;
                }
                else
                {
                    Node* root = i->node->leaves[0];
                    i->node->leaves[0] = root->leaves[1];
                    root->leaves[1] = i->node;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = 0;
                    root->balance = 0;
                }
                return;
            case 2:
                if(-1 == i->node->leaves[1]->balance)
                {
                    Node* middle = i->node->leaves[1];
                    Node* root = middle->leaves[0];
                    i->node->leaves[1] = root->leaves[0];
                    middle->leaves[0] = root->leaves[1];
                    root->leaves[0] = i->node;
                    root->leaves[1] = middle;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = -!(1 - root->balance);
                    middle->balance = !(1 + root->balance);
                    root->balance = 0;
                }
                else
                {
                    Node* root = i->node->leaves[1];
                    i->node->leaves[1] = root->leaves[0];
                    root->leaves[0] = i->node;
                    (i - 1)->node->leaves[(i - 1)->dir] = root;

                    i->node->balance = 0;
                    root->balance = 0;
                }
                return;
            }
        }
    }
    void init(int64 page_size)
    {
        assert(page_size);
        the_page_size = page_size + 1;
        the_head_page = core::acquire<Page>(sizeof(Page) + sizeof(Node) * (the_page_size - 1));
        the_head_page->next = 0;
        the_head_page->prev = 0;
        the_tail_page = the_head_page;
        the_tail = the_tail_page->data;
        the_root = 0;
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
                Page* page = the_tail_page;
                the_tail_page = the_tail_page->prev;
                the_tail = the_tail_page->data + the_page_size; // 0x0->data is 0x8 and it is NOT a bug
                release<Page>(page);
            }
            else
            {
                the_tail_page->next = 0;
                the_root = 0;
                return;
            }
        }
    }
    Node* acquire_tail()
    {
        if(the_tail == the_tail_page->data + the_page_size)
        {
            Page* page = the_tail_page;
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
            Page* page = the_tail_page;
            the_tail_page = the_tail_page->prev;
            the_tail_page->next = 0;
            the_tail = the_tail_page->data + the_page_size;
            release<Page>(page);
        }
    }
    int8 self_test(Node* root)
    {
        if(root)
        {
            auto h0 = self_test(root->leaves[0]);
            auto h1 = self_test(root->leaves[1]);
            assert(h1 - h0 == root->balance);
            return 1 + max(h0, h1);
        }
        return 0;
    }
private:
    Page* the_head_page;
    Page* the_tail_page;
    Node* the_tail;
    Node* the_root;
    int64 the_page_size;
    TreeIndex the_index;
};
