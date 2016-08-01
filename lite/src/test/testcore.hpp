
#ifndef TEST_CORE_HPP
#define TEST_CORE_HPP

#include "test/cs.hpp"
#include "core/format.hpp"
#include "core/queue.hpp"
#include "core/seq.hpp"
#include "core/list.hpp"
#include "core/hashset.hpp"
#include "core/hashmap.hpp"
#include "core/treeset.hpp"
#include "core/treemap.hpp"

struct Int2String
{
    core::int32 i;
    core::String st;
};

struct String2Int
{
    core::String st;
    core::int32 i;
};

class Flower
{
public:
    typedef core::Shared<Flower> share;
    typedef core::Managed<Flower> manage;
public:
    void init(core::uint32& cnt)
    {
        the_cnt = cnt;
        the_flower = cs_flower;
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
    core::share::uint32 the_cnt;
private:
    core::String the_flower;
};

class Poppy : public Flower
{
public:
    typedef core::Shared<Poppy> share;
    typedef core::Managed<Poppy> manage;
public:
    void init(core::uint32& cnt)
    {
        Flower::init(cnt);
        the_poppy = cs_poppy;
    }
    virtual ~Poppy()
    {
        the_cnt--;
    }
    virtual void virtual_method()
    {
        the_cnt--;
    }
private:
    core::String the_poppy;
};

inline void test_share()
{
    // trivial
    core::share::uint32 share_one = 1;
    core::share::uint32 share_two = share_one;
    share_one = 0;

    // complex
    core::String::share share_first = cs_first;
    core::String::share share_second = share_first;
    share_first = cs_third;
    core::verify(share_first->equal(cs_third));
    share_first = cs_third;

    // nil
    core::String::share share_st = core::nil;
    core::assert(share_st == core::nil);
    core::assert(core::nil == share_st);
    share_st = cs_notnil;
    core::verify(share_st != core::nil);
    core::verify(core::nil != share_st);

    // extending
    core::uint32 cnt = 3; // 2 * Flower + 1 * Poppy dtors
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
        }
        flower->virtual_method();
    }
    core::verify(!cnt);
}

inline void test_manage()
{
    // trivial
    core::manage::uint32 manage_one = 1;
    core::manage::uint32 manage_two = manage_one;
    manage_one = 0;

    // complex
    core::String::manage manage_first = cs_first;
    core::String::manage manage_second = manage_first;
    manage_first = cs_third.clone();
    core::verify(manage_first->equal(cs_third));
    manage_first = cs_third;

    // nil
    core::String::manage manage_st = cs_notnil;
    core::verify(manage_st != core::nil);
    core::verify(core::nil != manage_st);
    manage_st = core::nil;
    core::assert(manage_st == core::nil);
    core::assert(core::nil == manage_st);

    // extending
    core::uint32 cnt = 3; // 2 * Flower + 1 * Poppy dtors
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
        }
        flower->virtual_method();
    }
    core::verify(!cnt);
}

inline void test_string()
{
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
    core::String var_st = cs_punctuation;
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

    // core::String type
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
    core::uint32 pos = 0;
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
    st_copy.size(pos);

    // duplicate
    pos = 0;
    core::uint32 duplicate_size = cs_string.size();
    core::uint8 duplicate_space[duplicate_size];
    st_copy.copy_out(pos, duplicate_space, duplicate_size);
    core::String duplicate_st(duplicate_space, duplicate_size);
    core::verify(cs_string.equal(duplicate_st));

    // format
    core::String st_format = core::Format(cs_format)
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
    core::String::Iterator it = cs_hardcoded_output.head();
    core::verify(it.head());
    core::verify(!it.tail());
    while(it.next());
    core::verify(!it.head());
    core::verify(it.tail());
}

inline void test_acquire()
{
    try
    {
        core::acquire<core::uint32>(-1);
        core::verify(false);
    }
    catch(...)
    {

    }
}

inline void test_queue()
{
    const core::uint32 variation = 100;
    core::uint32 size = 2 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
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
    for(core::uint32 i = 0; i < size; i++)
    {
        balance += i;
        queue.push(core::String(i));
    }
    core::verify(size == queue.size());

    // pop partly
    for(core::uint32 i = 0; i < size / 2; i++)
    {
        core::String st;
        core::verify(queue.pop(st));
        balance -= st.to_uint32();
    }
    core::verify(size / 2 == queue.size());

    // push more
    for(core::uint32 i = 0; i < size; i++)
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
        balance -= st->to_uint32();

    core::verify(queue.is_empty());
    core::verify(!balance);
}

inline void test_seq()
{
    const core::uint32 variation = 100;
    core::uint32 size = 8 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
    core::Seq<core::String> seq;
    core::Seq<core::String>::share shared_seq = seq;
    core::Seq<core::String>::manage managed_seq;

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
    core::verify(core::uint32_nil == seq.search(core::nil));
    seq.sort();

    // init
    seq.size(size);

    // populate
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String rand(core::randomize() % size);
        seq.at(i) = rand;
        seq.append() = rand;
        balance += 4 * rand.to_uint32();
        balance += 2 * i + size;
    }

    // begin of verifying balance
    core::Seq<core::String>::Iterator it = seq.tail();
    core::verify(!it.head());
    core::verify(it.tail());
    while(it.prev())
    {
        balance -= it.value().to_uint32();
        balance -= it.position();
    }
    core::verify(it.head());
    core::verify(!it.tail());

    // sorting
    seq.sort();

    // modify sorted
    core::String put(core::randomize() % size);
    if(seq.put(put))
        balance += put.to_uint32();
    core::verify(seq.put(put, false));
    balance += put.to_uint32();
    core::uint32 erased = 0;
    while(!erased)
    {
        core::String erase(core::randomize() % size);
        erased = seq.erase(erase, false);
        balance -= erased * erase.to_uint32();
        core::verify(!seq.erase(erase));
    }

    // find
    core::String findy(core::randomize() % size);
    core::Seq<core::String>::Find find = seq.find(findy);
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
        core::String& next = it.value();
        core::verify(0 >= prev.compare(next));
        prev = next;
        balance -= next.to_uint32();
        if(findy.equal(next))
            balance--;
    }
    core::verify(!balance);

    // search
    for(core::uint32 i = 0; i < seq.size(); i++)
    {
        core::uint32 pos = core::randomize() % seq.size();
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
    core::uint32 cnt = seq.size();
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
    core::Seq<core::uint32> pos_seq;
    pos_seq.capacity(seq.size() / 2);
    it = seq.head();
    while(it.next())
    {
        core::uint32 pos = it.position();
        if(pos % 2)
            pos_seq.append(pos);
    }
    pos_seq.squeeze();
    seq.erase_by_iterator(pos_seq.head());
    core::verify(size / 8 == seq.size());

    // clean up
    seq.erase_all();
    core::verify(seq.is_empty());

    // stress
    core::uint32 scope = 1 + core::randomize() % 20;
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String put(core::randomize() % scope);
        core::String erase(core::randomize() % scope);
        seq.put(put, scope & 1);
        core::uint32 erasing = 0;
        core::Seq<core::String>::Find find = seq.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == seq.erase(erase, scope & 2) || scope & 2);
        core::verify(!seq.erase(erase) || scope & 2);
    }

    // int index
    typedef core::Index<core::int32> IntIndex;
    core::Seq<Int2String, IntIndex> int_seq;
    for(core::uint32 i = 0; i < size; i++)
    {
        Int2String i2s;
        i2s.i = core::randomize() % scope;
        i2s.st = core::String(core::randomize() % scope);
        int_seq.put(i2s);
    }
    core::int32 int_prev = 0;
    core::Seq<Int2String, IntIndex>::Iterator int_seq_it = int_seq.head();
    while(int_seq_it.next())
    {
        core::int32& int_next = (core::int32&)int_seq_it.value();
        core::verify(int_prev <= int_next);
        int_prev = int_next;
    }

    // String index
    typedef core::Index<core::String> StringIndex;
    core::Seq<String2Int, StringIndex> st_seq;
    for(core::uint32 i = 0; i < size; i++)
    {
        String2Int s2i;
        s2i.i = core::randomize() % scope;
        s2i.st = core::String(core::randomize() % scope);
        st_seq.put(s2i);
    }
    core::String st_prev;
    core::Seq<String2Int, StringIndex>::Iterator st_seq_it = st_seq.head();
    while(st_seq_it.next())
    {
        core::String& st_next = (core::String&)st_seq_it.value();
        core::verify(0 >= st_prev.compare(st_next));
        st_prev = st_next;
    }
}

inline void test_list()
{
    const core::uint32 variation = 100;
    core::uint32 size = 8 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
    core::List<core::String> list;
    core::List<core::String>::share shared_list = list;
    core::List<core::String>::manage managed_list;

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
    core::verify(core::uint32_nil == list.search(core::nil));
    list.sort();

    // populate
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String rand(core::randomize() % size);
        list.append() = rand;
        list.append() = rand;
        balance += 4 * rand.to_uint32();
        balance += 2 * i + size;
    }

    // begin of verifying balance
    core::List<core::String>::Iterator it = list.tail();
    core::verify(!it.head());
    core::verify(it.tail());
    while(it.prev())
    {
        balance -= it.value().to_uint32();
        balance -= it.position();
    }
    core::verify(it.head());
    core::verify(!it.tail());

    // sorting
    list.sort();

    // modify sorted
    core::String put(core::randomize() % size);
    if(list.put(put))
        balance += put.to_uint32();
    core::verify(list.put(put, false));
    balance += put.to_uint32();
    core::uint32 erased = 0;
    while(!erased)
    {
        core::String erase(core::randomize() % size);
        erased = list.erase(erase, false);
        balance -= erased * erase.to_uint32();
        core::verify(!list.erase(erase));
    }

    // find
    core::String findy(core::randomize() % size);
    core::List<core::String>::Find find = list.find(findy);
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
        core::String& next = it.value();
        core::verify(0 >= prev.compare(next));
        prev = next;
        balance -= next.to_uint32();
        if(findy.equal(next))
            balance--;
    }
    core::verify(!balance);

    // search
    for(core::uint32 i = 0; i < list.size(); i++)
    {
        core::uint32 pos = core::randomize() % list.size();
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
    core::uint32 cnt = list.size();
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
    core::List<core::uint32> pos_list(list.size() / 2);
    it = list.head();
    while(it.next())
    {
        core::uint32 pos = it.position();
        if(pos % 2) pos_list.append(pos);
    }
    list.erase_by_iterator(pos_list.head());
    core::verify(size / 8 == list.size());

    // clean up
    list.erase_all();
    core::verify(list.is_empty());

    // stress
    core::uint32 scope = 1 + core::randomize() % 20;
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String put(core::randomize() % scope);
        core::String erase(core::randomize() % scope);
        list.put(put, scope & 1);
        core::uint32 erasing = 0;
        core::List<core::String>::Find find = list.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == list.erase(erase, scope & 2) || scope & 2);
        core::verify(!list.erase(erase) || scope & 2);
    }

    // list as a pager
    core::List<core::String> pager(0x2);
    core::Seq<core::String::share> share_seq;
    core::List<core::String::share> share_list(0x2);
    core::HashSet<core::String::share> share_hash_set(0x2);
    core::HashMap<core::String, core::String::share> share_hash_map(0x2);
    core::TreeSet<core::String::share> share_tree_set(0x2);
    core::TreeMap<core::String, core::String::share> share_tree_map(0x2);

    for(core::uint32 i = 0; i < 100; i++)
    {
        core::String st(i);
        share_seq.acquire(st, pager);
        share_list.acquire(st, pager);
        share_hash_set.acquire(st, pager);
        share_hash_map.acquire(st, pager);
        share_tree_set.acquire(st, pager);
        share_tree_map.acquire(st, pager);
    }

    // int index, first field of Int2String is int
    typedef core::Index<core::int32> IntIndex;
    core::List<Int2String, IntIndex> int_list(0x5);
    for(core::uint32 i = 0; i < size; i++)
    {
        Int2String i2s;
        i2s.i = core::randomize() % scope;
        i2s.st = core::String(core::randomize() % scope);
        int_list.put(i2s);
    }
    core::int32 int_prev = 0;
    core::List<Int2String, IntIndex>::Iterator int_list_it = int_list.head();
    while(int_list_it.next())
    {
        core::int32& int_next = (core::int32&)int_list_it.value();
        core::verify(int_prev <= int_next);
        int_prev = int_next;
    }

    // String index, first field of String2Int is String
    typedef core::Index<core::String> StringIndex;
    core::List<String2Int, StringIndex> st_list(0x5);
    for(core::uint32 i = 0; i < size; i++)
    {
        String2Int s2i;
        s2i.i = core::randomize() % scope;
        s2i.st = core::String(core::randomize() % scope);
        st_list.put(s2i);
    }
    core::String st_prev;
    core::List<String2Int, StringIndex>::Iterator st_list_it = st_list.head();
    while(st_list_it.next())
    {
        core::String& st_next = (core::String&)st_list_it.value();
        core::verify(0 >= st_prev.compare(st_next));
        st_prev = st_next;
    }
}

inline void test_hash_set()
{
    const core::uint32 variation = 100;
    core::uint32 size = 2 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
    core::HashSet<core::String> set;
    core::HashSet<core::String>::share shared_set = set;
    core::HashSet<core::String>::manage managed_set;

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
    for(core::uint32 i = 0; i < size; i++)
    {
        balance += 2 * i;
        core::String value(i);
        core::verify(set.put(value));
        core::verify(set.set(value));
        core::verify(core::nil != set.lookup(value));
        core::verify(set.lookup(value)->equal(value));
    }
    core::verify(size == set.size());

    // put/erase multi-value
    core::uint32 erased = 0;
    while(erased < 2)
    {
        core::String rand(core::randomize() % size);
        core::verify(set.put(rand, false));
        core::verify(erased = set.erase(rand, false));
        core::verify(!set.erase(rand));
        for(core::uint32 i = 1; i < erased; i++)
            core::verify(set.put(rand, false));
    }

    // find
    core::String findy(core::randomize() % size);
    core::HashSet<core::String>::Find find = set.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.value()));
        balance++;
    }

    // verify balance
    core::HashSet<core::String>::Iterator it = set.head();
    while(it.next())
    {
        balance -= it.value().to_uint32();
        if(findy.equal(it.value()))
            balance--;
    }
    it = set.tail();
    core::verify(!it.head());
    core::verify(it.tail());
    while(it.prev())
        balance -= it.value().to_uint32();
    core::verify(it.head());
    core::verify(!it.tail());
    core::verify(!balance);

    // erase by key
    for(core::uint32 i = 0; i < size / 2; i++)
        core::verify(set.erase(core::String(i)));
    core::verify(!set.is_empty());
    core::verify(size / 2 == set.size());

    // erase all
    set.erase_all();
    core::verify(set.is_empty());

    // stress
    core::uint32 scope = 1 + core::randomize() % 20;
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String put(core::randomize() % scope);
        core::String erase(core::randomize() % scope);
        set.put(put, scope & 1);
        core::uint32 erasing = 0;
        core::HashSet<core::String>::Find find = set.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == set.erase(erase, scope & 2) || scope & 2);
        core::verify(!set.erase(erase) || scope & 2);
    }

    // int index, first field of Int2String is int
    typedef core::Index<core::int32> IntIndex;
    core::HashSet<Int2String, IntIndex> int_set(0x5);
    for(core::uint32 i = 0; i < size; i++)
    {
        Int2String i2s;
        i2s.i = core::randomize() % scope;
        i2s.st = core::String(core::randomize() % scope);
        int_set.put(i2s);
        core::verify(int_set.lookup(i2s)->i == i2s.i);
    }

    // String index, first field of String2Int is String
    typedef core::Index<core::String> StringIndex;
    core::HashSet<String2Int, StringIndex> st_set(0x5);
    for(core::uint32 i = 0; i < size; i++)
    {
        String2Int s2i;
        s2i.i = core::randomize() % scope;
        s2i.st = core::String(core::randomize() % scope);
        st_set.put(s2i);
        core::verify(st_set.lookup(s2i)->st.equal(s2i.st));
    }
}

inline void test_hash_map()
{
    const core::uint32 variation = 100;
    core::uint32 size = 2 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
    core::HashMap<core::String, core::uint32> map;
    core::HashMap<core::String, core::uint32>::share shared_map = map;
    core::HashMap<core::String, core::uint32>::manage managed_map;

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
    for(core::uint32 i = 0; i < size; i++)
    {
        balance += 2 * i;
        core::String key(balance);
        map.at(key) = i;
        core::verify(core::nil != map.lookup(key));
        core::verify(map.set(key, map.lookup(key)));
    }
    core::verify(size == map.size());

    // put/erase multi-value
    core::uint32 erased = 0;
    while(erased < 2)
    {
        core::String key(core::randomize() % size);
        core::uint32 value = core::nil != map.lookup(key)
            ? core::uint32(map.lookup(key))
            : 0;
        core::verify(map.put(key, key.to_uint32(), false));
        core::verify(erased = map.erase(key, false));
        core::verify(!map.erase(key));
        for(core::uint32 i = 1; i < erased; i++)
            core::verify(map.put(key, value, false));
    }

    // find
    core::String findy(core::randomize() % size);
    core::HashMap<core::String, core::uint32>::Find find = map.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.key()));
        balance++;
    }

    // verify balance
    core::HashMap<core::String, core::uint32>::Iterator it = map.head();
    while(it.next())
        balance -= it.value();
    it = map.tail();
    core::verify(!it.head());
    core::verify(it.tail());
    while(it.prev())
    {
        balance -= it.value();
        if(findy.equal(it.key()))
            balance--;
    }
    core::verify(it.head());
    core::verify(!it.tail());
    core::verify(!balance);

    // erase by key
    for(core::uint32 i = 0; i < size / 2; i++)
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
    core::uint32 scope = 1 + core::randomize() % 20;
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String put(core::randomize() % scope);
        core::String erase(core::randomize() % scope);
        map.put(put, put.to_uint32(), scope & 1);
        core::uint32 erasing = 0;
        core::HashMap<core::String, core::uint32>::Find find = map.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == map.erase(erase, scope & 2) || scope & 2);
        core::verify(!map.erase(erase) || scope & 2);
    }
}

inline void test_tree_set()
{
    const core::uint32 variation = 100;
    core::uint32 size = 2 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
    core::TreeSet<core::String> set;
    core::TreeSet<core::String>::share shared_set = set;
    core::TreeSet<core::String>::manage managed_set;

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

    // random populated
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String value(core::randomize() % variation);
        if(set.put(value))
        {
            balance += 2 * value.to_uint32();
            core::verify(set.set(value));
            core::verify(core::nil != set.lookup(value));
            core::verify(set.lookup(value)->equal(value));
        }
    }

    // put/erase multi-value
    core::String rand(core::randomize() % size);
    core::verify(set.put(rand, false));
    core::verify(set.put(rand, false));
    core::uint32 erased = set.erase(rand, false);
    core::verify(erased);
    core::verify(!set.erase(rand));
    for(core::uint32 i = 2; i < erased; i++)
        core::verify(set.put(rand, false));

    // find
    core::String findy(core::randomize() % size);
    core::TreeSet<core::String>::Find find = set.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.value()));
        balance++;
    }

    // read by sort
    core::String prev;
    core::TreeSet<core::String>::Sort sort = set.sort();
    while(sort.next())
    {
        core::String& next = sort.value();
        balance -= next.to_uint32();
        core::verify(0 >= prev.compare(next)); // verify value order
        prev = next;
        if(findy.equal(next))
            balance--;
    }
    sort = set.sort();
    while(sort.prev())
        balance -= sort.value().to_uint32();
    core::verify(!balance);

    // erase all
    set.erase_all();
    core::verify(set.is_empty());

    // populate
    for(core::uint32 i = 0; i < size; i++)
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
    core::TreeSet<core::String>::Iterator it = set.head();
    while(it.next())
        balance -= it.value().to_uint32();
    it = set.tail();
    core::verify(!it.head());
    core::verify(it.tail());
    while(it.prev())
        balance -= it.value().to_uint32();
    core::verify(it.head());
    core::verify(!it.tail());
    core::verify(!balance);

    // erase by key
    for(core::uint32 i = 0; i < size / 2; i++)
        core::verify(set.erase(core::String(i)));
    core::verify(!set.is_empty());
    core::verify(size / 2 == set.size());

    // erase all
    set.erase_all();
    core::verify(set.is_empty());

    // stress
    core::uint32 scope = 1 + core::randomize() % 20;
    for(core::uint32 i = 0; i < 10 * size; i++)
    {
        core::String put(core::randomize() % scope);
        core::String erase(core::randomize() % scope);
        set.put(put, scope & 1);
        core::uint32 erasing = 0;
        core::TreeSet<core::String>::Find find = set.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == set.erase(erase, scope & 2) || scope & 2);
        core::verify(!set.erase(erase) || scope & 2);
    }

    // int index, first field of Int2String is int
    typedef core::Index<core::int32> IntIndex;
    core::TreeSet<Int2String, IntIndex> int_set(0x5);
    for(core::uint32 i = 0; i < size; i++)
    {
        Int2String i2s;
        i2s.i = core::randomize() % scope;
        i2s.st = core::String(core::randomize() % scope);
        int_set.put(i2s);
    }
    core::int32 int_prev = 0;
    core::TreeSet<Int2String, IntIndex>::Sort int_set_sort = int_set.sort();
    while(int_set_sort.next())
    {
        core::int32& int_next = (core::int32&)int_set_sort.value();
        core::verify(int_prev <= int_next);
        int_prev = int_next;
    }

    // String index, first field of String2Int is String
    typedef core::Index<core::String> StringIndex;
    core::TreeSet<String2Int, StringIndex> st_set(0x5);
    for(core::uint32 i = 0; i < size; i++)
    {
        String2Int s2i;
        s2i.i = core::randomize() % scope;
        s2i.st = core::String(core::randomize() % scope);
        st_set.put(s2i);
    }
    core::String st_prev;
    core::TreeSet<String2Int, StringIndex>::Sort st_set_sort = st_set.sort();
    while(st_set_sort.next())
    {
        core::String& st_next = (core::String&)st_set_sort.value();
        core::verify(0 >= st_prev.compare(st_next));
        st_prev = st_next;
    }
}

inline void test_tree_map()
{
    const core::uint32 variation = 100;
    core::uint32 size = 2 * (core::randomize() % variation + 1);
    core::uint32 balance = 0;
    core::TreeMap<core::String, core::uint32> map;
    core::TreeMap<core::String, core::uint32>::share shared_map = map;
    core::TreeMap<core::String, core::uint32>::manage managed_map;

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

    // random populated
    for(core::uint32 i = 0; i < size; i++)
    {
        core::String key(core::randomize() % variation);
        if(map.put(key, i))
        {
            balance += 2 * key.to_uint32();
            core::verify(map.set(key, i));
            core::verify(core::nil != map.lookup(key));
            core::verify(i == map.lookup(key));
        }
    }

    // put/erase multi-value
    core::String key(core::randomize() % size);
    core::uint32 value = core::nil != map.lookup(key)
        ? core::uint32(map.lookup(key))
        : 0;
    core::verify(map.put(key, key.to_uint32(), false));
    core::verify(map.put(key, key.to_uint32(), false));
    core::uint32 erased = map.erase(key, false);
    core::verify(erased);
    core::verify(!map.erase(key));
    for(core::uint32 i = 2; i < erased; i++)
        core::verify(map.put(key, value, false));

    // find
    core::String findy(core::randomize() % size);
    core::TreeMap<core::String, core::uint32>::Find find = map.find(findy);
    while(find.next())
    {
        core::verify(findy.equal(find.key()));
        balance++;
    }

    // read by sort
    core::String prev;
    core::TreeMap<core::String, core::uint32>::Sort sort = map.sort();
    while(sort.next())
    {
        core::String& next = sort.key();
        balance -= next.to_uint32();
        core::verify(0 >= prev.compare(next)); // verify key order
        prev = next;
        if(findy.equal(next))
            balance--;
    }
    sort = map.sort();
    while(sort.prev())
        balance -= sort.key().to_uint32();
    core::verify(!balance);

    // erase all
    map.erase_all();
    core::verify(map.is_empty());

    // populate
    for(core::uint32 i = 0; i < size; i++)
    {
        balance += 2 * i;
        core::String key(balance);
        map.at(key) = i;
        core::verify(core::nil != map.lookup(key));
        core::verify(map.set(key, map.lookup(key)));
    }
    core::verify(size == map.size());

    // read in put order
    core::TreeMap<core::String, core::uint32>::Iterator it = map.head();
    while(it.next())
        balance -= it.value();
    it = map.tail();
    core::verify(!it.head());
    core::verify(it.tail());
    while(it.prev())
        balance -= it.value();
    core::verify(it.head());
    core::verify(!it.tail());
    core::verify(!balance);

    // erase by key
    for(core::uint32 i = 0; i < size / 2; i++)
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
    core::uint32 scope = 1 + core::randomize() % 20;
    for(core::uint32 i = 0; i < 10 * size; i++)
    {
        core::String put(core::randomize() % scope);
        core::String erase(core::randomize() % scope);
        map.put(put, i, scope & 1);
        core::uint32 erasing = 0;
        core::TreeMap<core::String, core::uint32>::Find find = map.find(erase);
        while(find.next())
            erasing++;
        core::verify(erasing == map.erase(erase, scope & 2) || scope & 2);
        core::verify(!map.erase(erase) || scope & 2);
    }
}

inline void test_core()
{
    test_share();
    test_manage();
    test_string();
    test_acquire();
    test_queue();
    for(core::uint32 i = 0; i < 2; i++)
    {
        test_seq();
        test_list();
        test_hash_set();
        test_hash_map();
        test_tree_set();
        test_tree_map();
    }
}

#endif // TEST_CORE_HPP

