
inline void test_types()
{
    core::verify((1 << 0) == sizeof(core::int8));
    core::verify((1 << 1) == sizeof(core::int16));
    core::verify((1 << 2) == sizeof(core::int32));
    core::verify((1 << 3) == sizeof(core::int64));

    core::verify((1 << 0) == sizeof(core::uint8));
    core::verify((1 << 1) == sizeof(core::uint16));
    core::verify((1 << 2) == sizeof(core::uint32));
    core::verify((1 << 3) == sizeof(core::uint64));

    core::verify((1 << 2) == sizeof(core::float32));
    core::verify((1 << 3) == sizeof(core::float64));
    core::verify((1 << 4) == sizeof(core::float128));
}

inline void test_acquire()
{
    try
    {
        core::acquire<core::float128>(core::int64_max);
        core::verify(false);
    }
    catch(...)
    {
        core::verify(true);
    }
}

inline void test_pager()
{
    core::Pager pager;
    core::String cs_hello = "hello";
    const core::uint32 size = 100;
    for(core::uint32 i = 0; i < size; i++)
    {
        core::float32& f = pager.acquire<core::float32>();
        f = 0;

        core::int32* int_seq = pager.acquire_seq<core::int32>(100);
        int_seq[0] = 0;
        int_seq[99] = 99;

        int_seq = pager.acquire_seq<core::int32>(200);
        int_seq[0] = 0;
        int_seq[199] = 199;

        core::String& st = pager.acquire<core::String>();
        st = cs_hello;

        core::String* st_seq = pager.acquire_seq<core::String>(2);
        st_seq[0] = cs_hello;
        st_seq[1] = cs_hello;
    }
}

class Flower
{
public:
    typedef core::Shared<Flower> share;
    typedef core::Managed<Flower> manage;
public:
    void init(core::int64& cnt)
    {
        the_cnt = cnt;
    }
    virtual ~Flower()
    {
        the_cnt--;
    }
    virtual void virtual_method()
    {
        the_cnt++;
    }
protected:
    core::share::int64 the_cnt;
};

class Poppy : public Flower
{
public:
    typedef core::Shared<Poppy> share;
    typedef core::Managed<Poppy> manage;
public:
    void init(core::int64& cnt)
    {
        Flower::init(cnt);
    }
    virtual ~Poppy()
    {
        the_cnt--;
    }
    virtual void virtual_method()
    {
        the_cnt--;
    }
};

core::String cs_first = "first";
core::String cs_third = "third";
core::String cs_notnil = "notnil";

inline void test_share()
{
    // trivial
    core::share::int64 share_one = 1;
    core::share::int64 share_two = share_one;
    core::verify(share_two != core::nil);
    share_one = 0;

    // complex
    core::String::share share_first = cs_first;
    core::String::share share_second = share_first;
    core::verify(share_second != core::nil);
    share_first = cs_third;
    core::verify(share_first->equal(cs_third));
    share_first = cs_third;

    // nil
    core::String::share share_st = core::nil;
    core::verify(share_st == core::nil);
    core::verify(core::nil == share_st);
    share_st = cs_notnil;
    core::verify(share_st != core::nil);
    core::verify(core::nil != share_st);

    // extending
    core::int64 cnt = 3; // 2 * Flower + 1 * Poppy dtors
    {
        Flower local_flower;
        Poppy local_poppy;
        Flower::share flower = local_flower;
        flower->init(cnt);
        flower->virtual_method();
        {
            Poppy::share poppy = local_poppy;
            poppy->init(cnt);
            core::verify(flower != poppy);
            core::verify(poppy != flower);
            flower = poppy;
            core::verify(flower == poppy);
            core::verify(poppy == flower);
            core::verify(flower.type_of<Poppy>());
            flower.down_cast<Poppy>();
        }
        flower->virtual_method();
    }
    core::verify(!cnt);
}

struct Triple
{
    Triple(core::int64 i, core::float64 f, core::String& st) : i(i), f(f), st(st)
    {

    }
    core::int64 i;
    core::float64 f;
    core::String st;
};

inline void test_manage()
{
    // trivial
    core::manage::int64 manage_one = 1;
    core::manage::int64 manage_two = manage_one;
    core::verify(manage_two != core::nil);
    manage_one = 0;

    // complex
    core::String::manage manage_first = cs_first;
    core::String::manage manage_second = manage_first;
    core::verify(manage_second != core::nil);
    manage_first = cs_third.clone();
    core::verify(manage_first->equal(cs_third));
    manage_first = cs_third;

    // nil
    core::String::manage manage_st = cs_notnil;
    core::verify(manage_st != core::nil);
    core::verify(core::nil != manage_st);
    manage_st = core::nil;
    core::verify(manage_st == core::nil);
    core::verify(core::nil == manage_st);

    // extending
    core::int64 cnt = 3; // 2 * Flower + 1 * Poppy dtors
    {
        Flower::manage flower;
        flower->init(cnt);
        flower->virtual_method();
        {
            Poppy::manage poppy;
            poppy->init(cnt);
            core::verify(flower != poppy);
            core::verify(poppy != flower);
            flower = poppy;
            core::verify(flower == poppy);
            core::verify(poppy == flower);
            core::verify(flower.type_of<Poppy>());
            flower.down_cast<Poppy>();
        }
        flower->virtual_method();
    }
    core::verify(!cnt);

    // variadic template constructor
    core::Managed<Triple> variadic(1, 2.0, "3");
    core::verify(1 == variadic->i);
    core::verify(2.0 == variadic->f);
    core::verify(variadic->st.equal("3"));
}

inline void test_iterable()
{
    // int
    auto balance = 0;
    for(auto& i : core::range(-1, 5))
    {
        balance += i;
    }
    core::verify(9 == balance);
    for(auto& i : core::reverse(core::range(-1, 5)))
    {
        balance += i;
    }
    core::verify(18 == balance);

    // seq, list
    core::Seq<core::int16> seq { 1, 2 };
    core::List<core::String> list { "1", "2" };
    for(auto& it : core::zip(seq, list))
    {
        auto value1 = it.it1().value();
        auto value2 = it.it2().value().to_int16();
        balance += value1 + value2;
        core::verify(value1 == value2);
    }
    for(auto& it : core::reverse(core::zip(seq, list)))
    {
        auto value1 = it.it1().value();
        auto value2 = it.it2().value().to_int16();
        balance += value1 + value2;
        core::verify(value1 == value2);
    }
    core::verify(30 == balance);

    // tree, hash set
    core::TreeSet<core::int16> tree_set { 1, 2 };
    core::HashSet<core::String> hash_set { "1", "2" };
    for(auto& it : core::zip(tree_set, hash_set))
    {
        auto value1 = it.it1().value();
        auto value2 = it.it2().value().to_int16();
        balance += value1 + value2;
        core::verify(value1 == value2);
    }
    for(auto& it : core::reverse(core::zip(tree_set, hash_set)))
    {
        auto value1 = it.it1().value();
        auto value2 = it.it2().value().to_int16();
        balance += value1 + value2;
        core::verify(value1 == value2);
    }
    core::verify(42 == balance);

    // tree, hash map
    core::TreeMap<core::int32, core::int16> tree_map {{ 1, 3 }, { 2, 4 }};
    core::HashMap<core::int32, core::String> hash_map {{ 1, "3" }, { 2, "4" }};
    for(auto& it : core::zip(tree_map, hash_map))
    {
        auto value1 = it.it1().value();
        auto value2 = it.it2().value().to_int16();
        balance += value1 + value2;
        core::verify(value1 == value2);
    }
    for(auto& it : core::reverse(core::zip(tree_map, hash_map)))
    {
        auto value1 = it.it1().value();
        auto value2 = it.it2().value().to_int16();
        balance += value1 + value2;
        core::verify(value1 == value2);
    }
    core::verify(70 == balance);
}

inline void test_string()
{
    core::String cs_string = " string: % %";
    core::String cs_data = " data: ";
    core::String cs_bool = " bool: ";
    core::String cs_int8 = " int8: ";
    core::String cs_int16 = " int16: ";
    core::String cs_int32 = " int32: ";
    core::String cs_int64 = " int64: ";
    core::String cs_uint8 = " uint8: ";
    core::String cs_uint16 = " uint16: ";
    core::String cs_uint32 = " uint32: ";
    core::String cs_uint64 = " uint64: ";
    core::String cs_float32 = " float32: ";
    core::String cs_float64 = " float64: ";
    core::String cs_float128 = " float128: ";
    core::String cs_and_the_end = " and the end%";
    core::String cs_mix = "-127false-9223372036854775807";
    core::String cs_format = " string: %% %%%1 data: %2 bool: %3 int8: %4 int16: %A int32: %6 int64: %7 "
            "uint8: %8 uint16: %9 uint32: %5 uint64: %B float32: %C float64: %D float128: %E and the end%%";
    core::String cs_hardcoded_output = " string: % %!\"£%^&*()_+{}[]:@~;'#|\\<>?,./ data: *** bool: false "
            "int8: -127 int16: -32767 int32: -2147483647 int64: -9223372036854775807 uint8: 127 uint16: 32767 "
            "uint32: 2147483647 uint64: 9223372036854775807 float32: 123456.78 float64: 1234567890.123456 "
            "float128: 1234567890.1234567 and the end%";

    // nil
    core::verify(core::nil == core::String());
    core::verify(core::String() == core::nil);
    core::verify(cs_notnil != core::nil);
    core::verify(core::nil != cs_notnil);

    // empty
    core::verify(core::String().is_empty());
    core::verify(!core::String().head().next());
    core::verify(!core::String().tail().prev());

    // basic types
    core::String var_st = "!\"£%^&*()_+{}[]:@~;'#|\\<>?,./";
    core::uint8 var_data[] = { '*', '*', '*' };
    bool var_bool = false;
    core::int8 var_int8 = -core::int8_max;
    core::int16 var_int16 = -core::int16_max;
    core::int32 var_int32 = -core::int32_max;
    core::int64 var_int64 = -core::int64_max;
    core::uint8 var_uint8 = core::int8_max;
    core::uint16 var_uint16 = core::int16_max;
    core::uint32 var_uint32 = core::int32_max;
    core::uint64 var_uint64 = core::int64_max;
    core::float32 var_float32 = 123456.78F;
    core::float64 var_float64 = 1234567890.123456;
    core::float128 var_float128 = 1234567890.1234567L;

    // core::String ctor
    core::String st(var_st);
    core::String st_data(var_data, sizeof(var_data));
    core::String st_bool(var_bool);
    core::String st_int8(var_int8);
    core::String st_int16(var_int16);
    core::String st_int32(var_int32);
    core::String st_int64(var_int64);
    core::String st_uint8(var_uint8);
    core::String st_uint16(var_uint16);
    core::String st_uint32(var_uint32);
    core::String st_uint64(var_uint64);
    core::String st_float32(var_float32, 2);
    core::String st_float64(var_float64, 6);
    core::String st_float128(var_float128, 7);
    auto size = st.size()
            + st_data.size()
            + st_int8.size()
            + st_int16.size()
            + st_int32.size()
            + st_int64.size()
            + st_uint8.size()
            + st_uint16.size()
            + st_uint32.size()
            + st_uint64.size()
            + st_float32.size()
            + st_float64.size()
            + st_float128.size();
    core::verify(155 == size);

    // basic String operations
    st_bool.prepend(var_int8).append(var_int64);
    st_int32 = st_bool;
    st_int64 = st_int32.clone();
    st_float128 = st_int64;
    st_float128.erase();
    core::verify(st_float128.is_empty());
    core::verify(st_int64.equal(cs_mix));

    // append
    core::String st_append;
    st_append.append(cs_string).append(var_st)
        .append(cs_data).append(var_data, sizeof(var_data))
        .append(cs_bool).append(var_bool)
        .append(cs_int8).append(var_int8)
        .append(cs_int16).append(var_int16)
        .append(cs_int32).append(var_int32)
        .append(cs_int64).append(var_int64)
        .append(cs_uint8).append(var_uint8)
        .append(cs_uint16).append(var_uint16)
        .append(cs_uint32).append(var_uint32)
        .append(cs_uint64).append(var_uint64)
        .append(cs_float32).append(var_float32, 2)
        .append(cs_float64).append(var_float64, 6)
        .append(cs_float128).append(var_float128, 7)
        .append(cs_and_the_end);

    // copy
    core::int64 pos = 0;
    core::String st_copy;
    st_copy.size(st_append.size());
    st_copy.copy_in(pos, cs_string);
    st_copy.copy_in(pos, st);
    st_copy.copy_in(pos, cs_data);
    st_copy.copy_in(pos, var_data, sizeof(var_data));
    st_copy.copy_in(pos, cs_bool);
    st_copy.copy_in(pos, var_bool);
    st_copy.copy_in(pos, cs_int8);
    st_copy.copy_in(pos, var_int8);
    st_copy.copy_in(pos, cs_int16);
    st_copy.copy_in(pos, var_int16);
    st_copy.copy_in(pos, cs_int32);
    st_copy.copy_in(pos, var_int32);
    st_copy.copy_in(pos, cs_int64);
    st_copy.copy_in(pos, var_int64);
    st_copy.copy_in(pos, cs_uint8);
    st_copy.copy_in(pos, var_uint8);
    st_copy.copy_in(pos, cs_uint16);
    st_copy.copy_in(pos, var_uint16);
    st_copy.copy_in(pos, cs_uint32);
    st_copy.copy_in(pos, var_uint32);
    st_copy.copy_in(pos, cs_uint64);
    st_copy.copy_in(pos, var_uint64);
    st_copy.copy_in(pos, cs_float32);
    st_copy.copy_in(pos, var_float32, 2);
    st_copy.copy_in(pos, cs_float64);
    st_copy.copy_in(pos, var_float64, 6);
    st_copy.copy_in(pos, cs_float128);
    st_copy.copy_in(pos, var_float128, 7);
    st_copy.copy_in(pos, cs_and_the_end);
    core::certify(pos == st_copy.size());
    st_copy.size(pos);

    // duplicate
    pos = 0;
    auto duplicate_size = cs_string.size();
    core::uint8 duplicate_space[duplicate_size];
    st_copy.copy_out(pos, duplicate_space, duplicate_size);
    core::String duplicate_st(duplicate_space, duplicate_size);
    core::verify(cs_string.equal(duplicate_st));

    // format
    auto st_format = core::Format(cs_format)
        .arg(var_st)
        .arg(var_data, sizeof(var_data))
        .arg(var_bool)
        .arg(var_int8)
        .arg(var_uint32)
        .arg(var_int32)
        .arg(var_int64)
        .arg(var_uint8)
        .arg(var_uint16)
        .arg(var_int16)
        .arg(var_uint64)
        .arg(var_float32, 2)
        .arg(var_float64, 6)
        .arg(var_float128, 7)
        .end();

    // result
    core::verify(cs_hardcoded_output.equal(st_append));
    core::verify(cs_hardcoded_output.equal(st_copy));
    core::verify(cs_hardcoded_output.equal(st_format));

    // iterator
    auto it = cs_hardcoded_output.head();
    core::verify(it.is_head());
    core::verify(!it.is_tail());
    while(it.next());
    core::verify(!it.is_head());
    core::verify(it.is_tail());

    // loop iterator
    auto position = 0;
    for(auto& it : cs_hardcoded_output)
    {
        core::verify(it.position() == position);
        core::verify(it.value() == cs_hardcoded_output[position]);
        position++;
    }
    core::verify(position == cs_hardcoded_output.size());
    for(auto& it : core::reverse(cs_hardcoded_output))
    {
        position--;
        core::verify(it.position() == position);
        core::verify(it.value() == cs_hardcoded_output[position]);
    }
    core::verify(position == 0);
}

inline void test_queue()
{
    auto variation = 100;
    auto size = 2 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::Queue<core::String> queue;
    queue.page_size(core::randomize() % variation + 1);

    // empty
    core::verify(queue.page_size());
    core::verify(!queue.size());
    core::verify(queue.is_empty());
    queue.erase_all();
    core::verify(!queue.pop());
    core::verify(core::nil == queue.peek());

    // push
    for(auto& i : core::range(size))
    {
        balance += i;
        queue.push(core::String(i));
    }
    core::verify(size == queue.size());

    // pop partly
    for(auto& i : core::range(size / 2))
    {
        core::String st;
        core::verify(queue.pop(st));
        auto value = st.to_int64();
        core::verify(i == value);
        balance -= value;
    }
    core::verify(size / 2 == queue.size());

    // push more
    for(auto& i : core::range(size))
    {
        balance += i;
        queue.push(core::String(i));
    }

    // peek
    core::String::share share = queue.peek();
    core::verify(core::nil != share);
    core::verify(queue.peek(share));

    // pop everything
    core::Managed<core::String> st;
    while(queue.pop(st))
        balance -= st->to_int64();

    core::verify(queue.is_empty());
    core::verify(!balance);
}

struct Int2String
{
    core::int64 i;
    core::String st;
};

struct String2Int
{
    core::String st;
    core::int64 i;
};

inline void test_seq()
{
    auto variation = 100;
    auto size = 8 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::Seq<core::String> seq;
    core::Seq<core::String>::share shared_seq = seq;
    core::Seq<core::String>::manage managed_seq;
    core::verify(shared_seq != core::nil);
    core::verify(managed_seq != core::nil);

    // empty
    core::verify(!seq.head().next());
    core::verify(!seq.tail().prev());
    core::verify(!seq.content());
    core::verify(!seq.capacity());
    seq.squeeze();
    core::verify(!seq.size());
    core::verify(seq.is_empty());
    seq.erase_all();
    core::verify(core::nil == seq.find(core::nil));
    core::verify(seq.find(core::nil) == core::nil);
    core::verify(core::int64_nil == seq.search(core::nil));
    seq.sort();

    // init
    seq.size(size);

    // populate
    for(auto& i : core::range(size))
    {
        core::String rand(core::randomize(i) % size);
        seq.at(i) = rand;
        seq.append() = rand;
        balance += 4 * rand.to_int64();
        balance += 2 * i + size;
    }

    // loop iterator
    auto position = 0;
    for(auto& it : seq)
    {
        core::verify(it.position() == position);
        core::verify(it.value() == seq[position]);
        position++;
    }
    core::verify(position == seq.size());
    for(auto& it : core::reverse(seq))
    {
        position--;
        core::verify(it.position() == position);
        core::verify(it.value() == seq[position]);
    }
    core::verify(position == 0);

    // begin of verifying balance
    auto it = seq.tail();
    core::verify(!it.is_head());
    core::verify(it.is_tail());
    while(it.prev())
    {
        balance -= it.value().to_int64();
        balance -= it.position();
    }
    core::verify(it.is_head());
    core::verify(!it.is_tail());

    // sorting
    seq.sort();

    // modify sorted
    core::String put(core::randomize() % size);
    if(seq.put(put))
        balance += put.to_int64();
    core::verify(seq.put(put, false));
    balance += put.to_int64();
    auto erased = 0;
    while(!erased)
    {
        core::String erase(core::randomize() % size);
        erased = seq.erase(erase, false);
        balance -= erased * erase.to_int64();
        core::verify(!seq.erase(erase));
    }

    // find
    core::String findy(core::randomize() % size);
    auto find = seq.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.value()));
        balance++;
    }

    // end of verifying balance after sorting
    core::String prev;
    it = seq.head();
    while(it.next())
    {
        auto& next = it.value();
        core::verify(0 >= prev.compare(next));
        prev = next;
        balance -= next.to_int64();
        if(findy.equal(next))
            balance--;
    }
    core::verify(!balance);

    // search
    for(auto& i : core::range(seq.size()))
    {
        auto pos = core::randomize(i) % seq.size();
        core::verify(seq.lookup(seq.at(pos))->equal(seq.at(pos)));
        core::verify(seq.at(seq.search(seq.at(pos))).equal(seq.at(pos)));
    }

    // change size
    seq.size(0);
    core::verify(!seq.size());
    seq.size(size);
    core::verify(size == seq.size());

    // limits put/erase
    seq.put(size, core::String(size));
    seq.put(0, core::String(0));
    seq.erase(0);
    seq.erase(size);
    core::verify(size == seq.size());

    // erase single
    auto cnt = seq.size();
    while(cnt)
    {
        cnt -= 2;
        seq.erase(cnt);
    }
    core::verify(size / 2 == seq.size());

    // erase and replace by tail
    cnt = seq.size();
    while(cnt)
    {
        cnt -= 2;
        seq.erase_by_tail(cnt);
    }
    core::verify(size / 4 == seq.size());

    // erase driven by sequence of positions
    core::Seq<core::int64> pos_seq;
    pos_seq.capacity(seq.size() / 2);
    it = seq.head();
    while(it.next())
    {
        auto pos = it.position();
        if(pos % 2)
            pos_seq.append(pos);
    }
    pos_seq.squeeze();
    seq.erase_by_iterable(pos_seq);
    core::verify(size / 8 == seq.size());

    // clean up
    seq.erase_all();
    core::verify(seq.is_empty());

    // stress
    auto scope = 1 + core::randomize() % 20;
    for(auto& i : core::range(size))
    {
        core::String put(core::randomize(i) % scope);
        core::String erase(core::randomize(i) % scope);
        seq.put(put, scope & 1);
        auto erasing = 0;
        auto find = seq.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == seq.erase(erase, scope & 2) || scope & 2);
        core::verify(!seq.erase(erase) || scope & 2);
        if(!seq.is_empty())
            core::verify(seq.last().equal(seq[seq.size()-1]));
    }

    // int index
    typedef core::Index<core::int64> IntIndex;
    core::Seq<Int2String, IntIndex> int_seq;
    for(auto& i : core::range(size))
    {
        Int2String i2s;
        i2s.i = core::randomize(i) % scope;
        i2s.st = core::String(core::randomize(i) % scope);
        int_seq.put(i2s);
    }
    auto int_prev = 0;
    auto int_seq_it = int_seq.head();
    while(int_seq_it.next())
    {
        auto int_next = int_seq_it.value().i;
        core::verify(int_prev <= int_next);
        int_prev = int_next;
    }

    // String index
    typedef core::Index<core::String> StringIndex;
    core::Seq<String2Int, StringIndex> st_seq;
    for(auto& i : core::range(size))
    {
        String2Int s2i;
        s2i.i = core::randomize(i) % scope;
        s2i.st = core::String(core::randomize(i) % scope);
        st_seq.put(s2i);
    }
    core::String st_prev;
    auto st_seq_it = st_seq.head();
    while(st_seq_it.next())
    {
        auto st_next = st_seq_it.value().st;
        core::verify(0 >= st_prev.compare(st_next));
        st_prev = st_next;
    }
}

inline void test_list()
{
    auto variation = 100;
    auto size = 8 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::List<core::String> list;
    core::List<core::String>::share shared_list = list;
    core::List<core::String>::manage managed_list;
    core::verify(shared_list != core::nil);
    core::verify(managed_list != core::nil);

    // init
    list.page_size(core::randomize() % variation + 1);

    // empty
    core::verify(!list.head().next());
    core::verify(!list.tail().prev());
    core::verify(list.page_size());
    core::verify(!list.size());
    core::verify(list.is_empty());
    list.erase_all();
    core::verify(core::nil == list.find(core::nil));
    core::verify(list.find(core::nil) == core::nil);
    core::verify(core::int64_nil == list.search(core::nil));
    list.sort();

    // populate
    for(auto& i : core::range(size))
    {
        core::String rand(core::randomize(i) % size);
        list.append() = rand;
        list.append() = rand;
        balance += 4 * rand.to_int64();
        balance += 2 * i + size;
    }

    // loop iterator
    auto position = 0;
    for(auto& it : list)
    {
        core::verify(it.position() == position);
        core::verify(it.value() == list[position]);
        position++;
    }
    core::verify(position == list.size());
    for(auto& it : core::reverse(list))
    {
        position--;
        core::verify(it.position() == position);
        core::verify(it.value() == list[position]);
    }
    core::verify(position == 0);

    // begin of verifying balance
    auto it = list.tail();
    core::verify(!it.is_head());
    core::verify(it.is_tail());
    while(it.prev())
    {
        balance -= it.value().to_int64();
        balance -= it.position();
    }
    core::verify(it.is_head());
    core::verify(!it.is_tail());

    // sorting
    list.sort();

    // modify sorted
    core::String put(core::randomize() % size);
    if(list.put(put))
        balance += put.to_int64();
    core::verify(list.put(put, false));
    balance += put.to_int64();
    auto erased = 0;
    while(!erased)
    {
        core::String erase(core::randomize() % size);
        erased = list.erase(erase, false);
        balance -= erased * erase.to_int64();
        core::verify(!list.erase(erase));
    }

    // find
    core::String findy(core::randomize() % size);
    auto find = list.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.value()));
        balance++;
    }

    // end of verifying balance after sorting
    core::String prev;
    it = list.head();
    while(it.next())
    {
        auto& next = it.value();
        core::verify(0 >= prev.compare(next));
        prev = next;
        balance -= next.to_int64();
        if(findy.equal(next))
            balance--;
    }
    core::verify(!balance);

    // search
    for(auto& i : core::range(list.size()))
    {
        auto pos = core::randomize(i) % list.size();
        core::verify(list.lookup(list.at(pos))->equal(list.at(pos)));
        core::verify(list.at(list.search(list.at(pos))).equal(list.at(pos)));
    }

    // limits put/erase
    list.put(0, core::String(0));
    list.put(list.size(), core::String(list.size()));
    list.erase(0);
    list.erase(list.size() - 1);
    while(size < list.size())
        list.erase(core::String(core::randomize() % size));
    core::verify(size == list.size());

    // erase single
    auto cnt = list.size();
    while(cnt)
    {
        cnt -= 2;
        list.erase(cnt);
    }
    core::verify(size / 2 == list.size());

    // erase and replace by tail
    cnt = list.size();
    while(cnt)
    {
        cnt -= 2;
        list.erase_by_tail(cnt);
    }
    core::verify(size / 4 == list.size());

    // erase driven by list of positions
    core::List<core::int64> pos_list(list.size() / 2);
    it = list.head();
    while(it.next())
    {
        auto pos = it.position();
        if(pos % 2) pos_list.append(pos);
    }
    list.erase_by_iterable(pos_list);
    core::verify(size / 8 == list.size());

    // clean up
    list.erase_all();
    core::verify(list.is_empty());

    // stress
    auto scope = 1 + core::randomize() % 20;
    for(auto& i : core::range(size))
    {
        core::String put(core::randomize(i) % scope);
        core::String erase(core::randomize(i) % scope);
        list.put(put, scope & 1);
        auto erasing = 0;;
        auto find = list.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == list.erase(erase, scope & 2) || scope & 2);
        core::verify(!list.erase(erase) || scope & 2);
        if(!list.is_empty())
            core::verify(list.last().equal(list[list.size()-1]));
    }

    // list as a pager
    core::List<core::String> pager(0x2);
    core::Seq<core::String::share> share_seq;
    core::List<core::String::share> share_list(0x2);
    core::HashSet<core::String::share> share_hash_set(0x2);
    core::HashMap<core::String, core::String::share> share_hash_map(0x2);
    core::TreeSet<core::String::share> share_tree_set(0x2);
    core::TreeMap<core::String, core::String::share> share_tree_map(0x2);

    for(auto& i : core::range(100))
    {
        // TODO: needs verification
        core::String st(i);
        share_seq.acquire(st, pager);
        share_list.acquire(st, pager);
        share_hash_set.acquire(st, pager);
        share_hash_map.acquire(st, pager);
        share_tree_set.acquire(st, pager);
        share_tree_map.acquire(st, pager);
    }

    // int index, first field of Int2String is int
    typedef core::Index<core::int64> IntIndex;
    core::List<Int2String, IntIndex> int_list(0x5);
    for(auto& i : core::range(size))
    {
        Int2String i2s;
        i2s.i = core::randomize(i) % scope;
        i2s.st = core::String(core::randomize(i) % scope);
        int_list.put(i2s);
    }
    auto int_prev = 0;
    auto int_list_it = int_list.head();
    while(int_list_it.next())
    {
        auto int_next = int_list_it.value().i;
        core::verify(int_prev <= int_next);
        int_prev = int_next;
    }

    // String index, first field of String2Int is String
    typedef core::Index<core::String> StringIndex;
    core::List<String2Int, StringIndex> st_list(0x5);
    for(auto& i : core::range(size))
    {
        String2Int s2i;
        s2i.i = core::randomize(i) % scope;
        s2i.st = core::String(core::randomize(i) % scope);
        st_list.put(s2i);
    }
    core::String st_prev;
    auto st_list_it = st_list.head();
    while(st_list_it.next())
    {
        auto st_next = st_list_it.value().st;
        core::verify(0 >= st_prev.compare(st_next));
        st_prev = st_next;
    }
}

inline void test_hash_set()
{
    auto variation = 100;
    auto size = 2 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::HashSet<core::String> set;
    core::HashSet<core::String>::share shared_set = set;
    core::HashSet<core::String>::manage managed_set;
    core::verify(shared_set != core::nil);
    core::verify(managed_set != core::nil);

    // init
    set.page_size(core::randomize() % variation + 1);

    // empty
    core::verify(!set.head().next());
    core::verify(!set.tail().prev());
    core::verify(set.page_size());
    core::verify(!set.size());
    core::verify(set.is_empty());
    core::verify(core::nil == set.find(core::nil));
    core::verify(set.find(core::nil) == core::nil);
    core::verify(!set.find(core::nil).next());
    set.erase_all();

    // populate
    for(auto& i : core::range(size))
    {
        balance += 2 * i;
        core::String value(i);
        core::verify(set.put(value));
        core::verify(set.set(value));
        core::verify(core::nil != set.lookup(value));
        core::verify(set.lookup(value)->equal(value));
    }
    core::verify(size == set.size());

    // loop iterator
    auto position = 0;
    for(auto& it : set)
    {
        core::verify(it.value() == set[it.value()]);
        position++;
    }
    core::verify(position == set.size());
    for(auto& it : core::reverse(set))
    {
        core::verify(it.value() == set[it.value()]);
        position--;
    }
    core::verify(position == 0);

    // put/erase multi-value
    auto erased = 0;
    while(erased < 2)
    {
        core::String rand(core::randomize() % size);
        core::verify(set.put(rand, false));
        core::verify(erased = set.erase(rand, false));
        core::verify(!set.erase(rand));
        for(auto i = 1; i < erased; i++)
            core::verify(set.put(rand, false));
    }

    // find
    core::String findy(core::randomize() % size);
    auto find = set.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.value()));
        balance++;
    }

    // verify balance
    auto it = set.head();
    while(it.next())
    {
        balance -= it.value().to_int64();
        if(findy.equal(it.value()))
            balance--;
    }
    it = set.tail();
    core::verify(!it.is_head());
    core::verify(it.is_tail());
    while(it.prev())
        balance -= it.value().to_int64();
    core::verify(it.is_head());
    core::verify(!it.is_tail());
    core::verify(!balance);

    // erase by key
    for(auto& i : core::range(size / 2))
        core::verify(set.erase(core::String(i)));
    core::verify(!set.is_empty());
    core::verify(size / 2 == set.size());

    // erase all
    set.erase_all();
    core::verify(set.is_empty());

    // stress
    auto scope = 1 + core::randomize() % 20;
    for(auto& i : core::range(size))
    {
        core::String put(core::randomize(i) % scope);
        core::String erase(core::randomize(i) % scope);
        set.put(put, scope & 1);
        auto erasing = 0;;
        auto find = set.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == set.erase(erase, scope & 2) || scope & 2);
        core::verify(!set.erase(erase) || scope & 2);
    }

    // int index, first field of Int2String is int
    typedef core::Index<core::int64> IntIndex;
    core::HashSet<Int2String, IntIndex> int_set(0x5);
    for(auto& i : core::range(size))
    {
        Int2String i2s;
        i2s.i = core::randomize(i) % scope;
        i2s.st = core::String(core::randomize(i) % scope);
        int_set.put(i2s);
        core::verify(int_set.lookup(i2s)->i == i2s.i);
    }

    // String index, first field of String2Int is String
    typedef core::Index<core::String> StringIndex;
    core::HashSet<String2Int, StringIndex> st_set(0x5);
    for(auto& i : core::range(size))
    {
        String2Int s2i;
        s2i.i = core::randomize(i) % scope;
        s2i.st = core::String(core::randomize(i) % scope);
        st_set.put(s2i);
        core::verify(st_set.lookup(s2i)->st.equal(s2i.st));
    }
}

inline void test_hash_map()
{
    auto variation = 100;
    auto size = 2 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::HashMap<core::String, core::int64> map;
    core::HashMap<core::String, core::int64>::share shared_map = map;
    core::HashMap<core::String, core::int64>::manage managed_map;
    core::verify(shared_map != core::nil);
    core::verify(managed_map != core::nil);

    // init
    map.page_size(core::randomize() % variation + 1);

    // empty
    core::verify(!map.head().next());
    core::verify(!map.tail().prev());
    core::verify(map.page_size());
    core::verify(!map.size());
    core::verify(map.is_empty());
    core::verify(core::nil == map.find(core::nil));
    core::verify(map.find(core::nil) == core::nil);
    core::verify(!map.find(core::nil).next());
    map.erase_all();

    // populate
    for(auto& i : core::range(size))
    {
        balance += 2 * i;
        core::String key(balance);
        map.at(key) = i;
        core::verify(core::nil != map.lookup(key));
        core::verify(map.set(key, map.lookup(key)));
    }
    core::verify(size == map.size());

    // loop iterator
    auto position = 0;
    for(auto& it : map)
    {
        core::verify(it.value() == map[it.key()]);
        position++;
    }
    core::verify(position == map.size());
    for(auto& it : core::reverse(map))
    {
        core::verify(it.value() == map[it.key()]);
        position--;
    }
    core::verify(position == 0);

    // put/erase multi-value
    auto erased = 0;
    while(erased < 2)
    {
        core::String key(core::randomize() % size);
        auto value = core::nil != map.lookup(key)
            ? core::int64(map.lookup(key))
            : 0;
        core::verify(map.put(key, key.to_int64(), false));
        core::verify(erased = map.erase(key, false));
        core::verify(!map.erase(key));
        for(auto i = 1; i < erased; i++)
            core::verify(map.put(key, value, false));
    }

    // find
    core::String findy(core::randomize() % size);
    auto find = map.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.key()));
        balance++;
    }

    // verify balance
    auto it = map.head();
    while(it.next())
        balance -= it.value();
    it = map.tail();
    core::verify(!it.is_head());
    core::verify(it.is_tail());
    while(it.prev())
    {
        balance -= it.value();
        if(findy.equal(it.key()))
            balance--;
    }
    core::verify(it.is_head());
    core::verify(!it.is_tail());
    core::verify(!balance);

    // erase by key
    for(auto& i : core::range(size / 2))
    {
        balance += 2 * i;
        core::String key(balance);
        core::verify(map.erase(key));
    }
    core::verify(!map.is_empty());
    core::verify(size / 2 == map.size());

    // erase all
    map.erase_all();
    core::verify(map.is_empty());

    // stress
    auto scope = 1 + core::randomize() % 20;
    for(auto& i : core::range(size))
    {
        core::String put(core::randomize(i) % scope);
        core::String erase(core::randomize(i) % scope);
        map.put(put, put.to_int64(), scope & 1);
        auto erasing = 0;;
        auto find = map.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == map.erase(erase, scope & 2) || scope & 2);
        core::verify(!map.erase(erase) || scope & 2);
    }
}

inline void test_tree_set()
{
    auto variation = 100;
    auto size = 2 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::TreeSet<core::String> set;
    core::TreeSet<core::String>::share shared_set = set;
    core::TreeSet<core::String>::manage managed_set;
    core::verify(shared_set != core::nil);
    core::verify(managed_set != core::nil);

    // init
    set.page_size(core::randomize() % variation + 1);

    // empty
    core::verify(!set.head().next());
    core::verify(!set.tail().prev());
    core::verify(set.page_size());
    core::verify(!set.size());
    core::verify(set.is_empty());
    core::verify(core::nil == set.find(core::nil));
    core::verify(set.find(core::nil) == core::nil);
    core::verify(!set.find(core::nil).next());
    core::verify(!set.sort().next());
    set.erase_all();

    // randomly populated
    for(auto& i : core::range(size))
    {
        core::String value(core::randomize(i) % variation);
        if(set.put(value))
        {
            balance += 2 * value.to_int64();
            core::verify(set.set(value));
            core::verify(core::nil != set.lookup(value));
            core::verify(set.lookup(value)->equal(value));
        }
    }

    // loop iterator
    auto position = 0;
    for(auto& it : set)
    {
        core::verify(it.value() == set[it.value()]);
        position++;
    }
    core::verify(position == set.size());
    for(auto& it : core::reverse(set))
    {
        core::verify(it.value() == set[it.value()]);
        position--;
    }
    core::verify(position == 0);

    // put/erase multi-value
    core::String rand(core::randomize() % size);
    core::verify(set.put(rand, false));
    core::verify(set.put(rand, false));
    auto erased = set.erase(rand, false);
    core::verify(erased);
    core::verify(!set.erase(rand));
    for(auto i = 2; i < erased; i++)
        core::verify(set.put(rand, false));

    // find
    core::String findy(core::randomize() % size);
    auto find = set.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.value()));
        balance++;
    }

    // read by sort
    core::String prev;
    auto sort = set.sort();
    while(sort.next())
    {
        auto& next = sort.value();
        balance -= next.to_int64();
        core::verify(0 >= prev.compare(next)); // verify value order
        prev = next;
        if(findy.equal(next))
            balance--;
    }
    sort = set.sort();
    while(sort.prev())
        balance -= sort.value().to_int64();
    core::verify(!balance);

    // erase all
    set.erase_all();
    core::verify(set.is_empty());

    // populate
    for(auto& i : core::range(size))
    {
        balance += 2 * i;
        core::String value(i);
        core::verify(set.put(value));
        core::verify(set.set(value));
        core::verify(core::nil != set.lookup(value));
        core::verify(set.lookup(value)->equal(value));
        core::verify(set.at(value).equal(value));
    }
    core::verify(size == set.size());

    // read in put order
    auto it = set.head();
    while(it.next())
        balance -= it.value().to_int64();
    it = set.tail();
    core::verify(!it.is_head());
    core::verify(it.is_tail());
    while(it.prev())
        balance -= it.value().to_int64();
    core::verify(it.is_head());
    core::verify(!it.is_tail());
    core::verify(!balance);

    // erase by key
    for(auto& i : core::range(size / 2))
        core::verify(set.erase(core::String(i)));
    core::verify(!set.is_empty());
    core::verify(size / 2 == set.size());

    // erase all
    set.erase_all();
    core::verify(set.is_empty());

    // stress
    auto scope = 1 + core::randomize() % 20;
    for(auto& i : core::range(10 * size))
    {
        core::String put(core::randomize(i) % scope);
        core::String erase(core::randomize(i) % scope);
        set.put(put, scope & 1);
        auto erasing = 0;;
        auto find = set.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == set.erase(erase, scope & 2) || scope & 2);
        core::verify(!set.erase(erase) || scope & 2);
    }

    // int index, first field of Int2String is int
    typedef core::Index<core::int64> IntIndex;
    core::TreeSet<Int2String, IntIndex> int_set(0x5);
    for(auto& i : core::range(size))
    {
        Int2String i2s;
        i2s.i = core::randomize(i) % scope;
        i2s.st = core::String(core::randomize(i) % scope);
        int_set.put(i2s);
    }
    auto int_prev = 0;
    auto int_set_sort = int_set.sort();
    while(int_set_sort.next())
    {
        auto int_next = int_set_sort.value().i;
        core::verify(int_prev <= int_next);
        int_prev = int_next;
    }

    // String index, first field of String2Int is String
    typedef core::Index<core::String> StringIndex;
    core::TreeSet<String2Int, StringIndex> st_set(0x5);
    for(auto& i : core::range(size))
    {
        String2Int s2i;
        s2i.i = core::randomize(i) % scope;
        s2i.st = core::String(core::randomize(i) % scope);
        st_set.put(s2i);
    }
    core::String st_prev;
    auto st_set_sort = st_set.sort();
    while(st_set_sort.next())
    {
        auto st_next = st_set_sort.value().st;
        core::verify(0 >= st_prev.compare(st_next));
        st_prev = st_next;
    }
}

inline void test_tree_map()
{
    auto variation = 100;
    auto size = 2 * (core::randomize() % variation + 1);
    auto balance = 0;
    core::TreeMap<core::String, core::int64> map;
    core::TreeMap<core::String, core::int64>::share shared_map = map;
    core::TreeMap<core::String, core::int64>::manage managed_map;
    core::verify(shared_map != core::nil);
    core::verify(managed_map != core::nil);

    // init
    map.page_size(core::randomize() % variation + 1);

    // empty
    core::verify(!map.head().next());
    core::verify(!map.tail().prev());
    core::verify(map.page_size());
    core::verify(!map.size());
    core::verify(map.is_empty());
    core::verify(core::nil == map.find(core::nil));
    core::verify(map.find(core::nil) == core::nil);
    core::verify(!map.find(core::nil).next());
    core::verify(!map.sort().next());
    map.erase_all();

    // randomly populated
    for(auto& i : core::range(size))
    {
        core::String key(core::randomize(i) % variation);
        if(map.put(key, i))
        {
            balance += 2 * key.to_int64();
            core::verify(map.set(key, i));
            core::verify(core::nil != map.lookup(key));
            core::verify(i == map.lookup(key));
        }
    }

    // loop iterator
    auto position = 0;
    for(auto& it : map)
    {
        core::verify(it.value() == map[it.key()]);
        position++;
    }
    core::verify(position == map.size());
    for(auto& it : core::reverse(map))
    {
        core::verify(it.value() == map[it.key()]);
        position--;
    }
    core::verify(position == 0);

    // put/erase multi-value
    core::String key(core::randomize() % size);
    auto value = core::nil != map.lookup(key)
        ? core::int64(map.lookup(key))
        : 0;
    core::verify(map.put(key, key.to_int64(), false));
    core::verify(map.put(key, key.to_int64(), false));
    auto erased = map.erase(key, false);
    core::verify(erased);
    core::verify(!map.erase(key));
    for(auto i = 2; i < erased; i++)
        core::verify(map.put(key, value, false));

    // find
    core::String findy(core::randomize() % size);
    auto find = map.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.key()));
        balance++;
    }

    // read by sort
    core::String prev;
    auto sort = map.sort();
    while(sort.next())
    {
        auto& next = sort.key();
        balance -= next.to_int64();
        core::verify(0 >= prev.compare(next)); // verify key order
        prev = next;
        if(findy.equal(next))
            balance--;
    }
    sort = map.sort();
    while(sort.prev())
        balance -= sort.key().to_int64();
    core::verify(!balance);

    // erase all
    map.erase_all();
    core::verify(map.is_empty());

    // populate
    for(auto& i : core::range(size))
    {
        balance += 2 * i;
        core::String key(balance);
        map.at(key) = i;
        core::verify(core::nil != map.lookup(key));
        core::verify(map.set(key, map.lookup(key)));
    }
    core::verify(size == map.size());

    // read in put order
    auto it = map.head();
    while(it.next())
        balance -= it.value();
    it = map.tail();
    core::verify(!it.is_head());
    core::verify(it.is_tail());
    while(it.prev())
        balance -= it.value();
    core::verify(it.is_head());
    core::verify(!it.is_tail());
    core::verify(!balance);

    // erase by key
    for(auto& i : core::range(size / 2))
    {
        balance += 2 * i;
        core::String key(balance);
        core::verify(map.erase(key));
    }
    core::verify(!map.is_empty());
    core::verify(size / 2 == map.size());

    // erase all
    map.erase_all();
    core::verify(map.is_empty());

    // stress
    auto scope = 1 + core::randomize() % 20;
    for(auto& i : core::range(10 * size))
    {
        core::String put(core::randomize(i) % scope);
        core::String erase(core::randomize(i) % scope);
        map.put(put, i, scope & 1);
        auto erasing = 0;;
        auto find = map.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == map.erase(erase, scope & 2) || scope & 2);
        core::verify(!map.erase(erase) || scope & 2);
    }
}

inline void test_core()
{
    test_types();
    test_pager();
    test_share();
    test_manage();
    test_iterable();
    test_string();
    test_acquire();
    test_queue();
    for(auto i = 0; i < 2; i++)
    {
        test_seq();
        test_list();
        test_hash_set();
        test_hash_map();
        test_tree_set();
        test_tree_map();
    }
}
