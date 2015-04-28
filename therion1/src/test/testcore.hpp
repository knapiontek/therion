
#ifndef TEST_CORE_HPP
#define TEST_CORE_HPP

#include "test/cs.hpp"
#include "core/core/format.hpp"
#include "core/core/pager.hpp"
#include "core/core/seq.hpp"
#include "core/core/list.hpp"
#include "core/core/queue.hpp"
#include "core/core/hashset.hpp"
#include "core/core/hashmap.hpp"
#include "core/core/treeset.hpp"
#include "core/core/treemap.hpp"

class Clazz
{
public:
	typedef core::Shared<Clazz> share;
	typedef core::Managed<Clazz> manage;
public:
	void init(core::uint32& cnt)
	{
		the_cnt = cnt;
		the_clazz = cs_clazz;
	}
	virtual ~Clazz()
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
	core::String the_clazz;
};

class Extension : public Clazz
{
public:
	typedef core::Shared<Extension> share;
	typedef core::Managed<Extension> manage;
public:
	void init(core::uint32& cnt)
	{
		Clazz::init(cnt);
		the_extension = cs_extension;
	}
	virtual ~Extension()
	{
		the_cnt--;
	}
	virtual void virtual_method()
	{
		the_cnt--;
	}
private:
	core::String the_extension;
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
	core::String::share nil_st = core::String::share::nil();
	core::assert(core::nil(nil_st));
	core::assert(!core::is(nil_st));
	core::String::share share_st;
	core::verify(share_st == core::String::share::nil());

	// extending
	core::uint32 cnt = 3; // 2 * Clazz + 1 * Extension dtors
	{
		Clazz local_clazz;
		Extension local_extension;
		Clazz::share clazz = local_clazz;
		clazz->init(cnt);
		clazz->virtual_method();
		{
			Extension::share extension = local_extension;
			extension->init(cnt);
			clazz = extension;
			core::verify(clazz == extension);
		}
		clazz->virtual_method();
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
	core::String::manage nil_st = core::String::manage::nil();
	core::assert(core::nil(nil_st));
	core::assert(!core::is(nil_st));
	core::String::manage manage_st;
	core::verify(manage_st != core::String::manage::nil());

	// extending
	core::uint32 cnt = 3; // 2 * Clazz + 1 * Extension dtors
	{
		Clazz::manage clazz;
		clazz->init(cnt);
		clazz->virtual_method();
		{
			Extension::manage extension;
			extension->init(cnt);
			clazz = extension;
			core::verify(clazz == extension);
		}
		clazz->virtual_method();
	}
	core::verify(!cnt);
}

inline void test_string()
{
	// nil
	core::verify(core::String() == core::String::nil());
	core::verify(cs_notnil != core::String::nil());

	// empty
	core::verify(!core::String().head().next());
	core::verify(!core::String().tail().prev());

	// basic types
	core::String var_st = cs_punctuation;
	core::Region var_region = s2r(cs_region_data);
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
	core::String st_region(var_region);
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
		.append(cs_region).append(var_region)
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
	st_copy.copy_in(pos, cs_region);
	st_copy.copy_in(pos, var_region);
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
	core::Region duplicate_region = b2r(duplicate_space);
	st_copy.copy_out(pos, duplicate_region);
	core::verify(cs_string.equal(core::String(duplicate_region)));

	// format
	core::String st_format = core::Format(cs_format)
		.arg(var_st)
		.arg(var_region)
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

inline void test_pager()
{
	core::Pager pager;
	const core::uint32 size = 100;
	for(core::uint32 i = 0; i < size; i++)
	{
		core::float32* f = pager.acquire_nil<core::float32>();
		*f = 0;

		core::int32* is = pager.acquire<core::int32>(100);
		is[0] = 0;
		is[99] = 99;
		is = pager.acquire<core::int32>(200);
		is[0] = 0;
		is[199] = 199;

		typedef core::HashSet<core::int32> HashSet;

		HashSet* set = pager.create<HashSet>();
		set->page_size(0x1);
		HashSet* sets = pager.create<HashSet>(2);
		sets[0].page_size(0x2);
		sets[1].page_size(0x2);
	}
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

inline void test_seq()
{
	const core::uint32 variation = 100;
	core::uint32 size = 8 * (core::randomize() % variation + 1);
	core::uint32 balance = 0;
	core::Seq<core::String> seq;

	// empty
	core::verify(!seq.head().next());
	core::verify(!seq.tail().prev());
	core::verify(!seq.content());
	core::verify(!seq.capacity());
	seq.squeeze();
	core::verify(!seq.size());
	core::verify(seq.is_empty());
	seq.erase_all();
	core::verify(core::uint32_nil == seq.search(core::String()));
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
		if(pos % 2) pos_seq.append(pos);
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
}

inline void test_list()
{
	const core::uint32 variation = 100;
	core::uint32 size = 8 * (core::randomize() % variation + 1);
	core::uint32 balance = 0;
	core::List<core::String> list;
	list.page_size(core::randomize() % variation + 1);

	// empty
	core::verify(!list.head().next());
	core::verify(!list.tail().prev());
	core::verify(list.page_size());
	core::verify(list.page_size());
	core::verify(!list.size());
	core::verify(list.is_empty());
	list.erase_all();
	core::verify(core::uint32_nil == list.search(core::String()));
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
	core::verify(core::nil(queue.peek()));

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
	core::verify(core::is(share));
	core::verify(queue.peek(share));

	// pop everything
	core::Managed<core::String> st;
	while(queue.pop(st))
		balance -= st->to_uint32();

	core::verify(queue.is_empty());
	core::verify(!balance);
}

inline void test_hash_set()
{
	const core::uint32 variation = 100;
	core::uint32 size = 2 * (core::randomize() % variation + 1);
	core::uint32 balance = 0;
	core::HashSet<core::String> set;
	set.page_size(core::randomize() % variation + 1);

	// empty
	core::verify(!set.head().next());
	core::verify(!set.tail().prev());
	core::verify(set.page_size());
	core::verify(!set.size());
	core::verify(set.is_empty());
	core::verify(!set.find(core::String()).next());
	set.erase_all();

	// populate
	for(core::uint32 i = 0; i < size; i++)
	{
		balance += 2 * i;
		core::String value(i);
		core::verify(set.put(value));
		core::verify(set.set(value));
		core::verify(core::is(set.lookup(value)));
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
}

inline void test_hash_map()
{
	const core::uint32 variation = 100;
	core::uint32 size = 2 * (core::randomize() % variation + 1);
	core::uint32 balance = 0;
	core::HashMap<core::String, core::uint32> map;
	map.page_size(core::randomize() % variation + 1);

	// empty
	core::verify(!map.head().next());
	core::verify(!map.tail().prev());
	core::verify(map.page_size());
	core::verify(!map.size());
	core::verify(map.is_empty());
	core::verify(!map.find(core::String()).next());
	map.erase_all();

	// populate
	for(core::uint32 i = 0; i < size; i++)
	{
		balance += 2 * i;
		core::String key(balance);
		map.at(key) = i;
		core::verify(core::is(map.lookup(key)));
		core::verify(map.set(key, map.lookup(key)));
	}
	core::verify(size == map.size());

	// put/erase multi-value
	core::uint32 erased = 0;
	while(erased < 2)
	{
		core::String key(core::randomize() % size);
		core::uint32 value = core::is(map.lookup(key))
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
	set.page_size(core::randomize() % variation + 1);

	// empty
	core::verify(!set.head().next());
	core::verify(!set.tail().prev());
	core::verify(set.page_size());
	core::verify(!set.size());
	core::verify(set.is_empty());
	core::verify(!set.find(core::String()).next());
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
			core::verify(core::is(set.lookup(value)));
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
		core::verify(core::is(set.lookup(value)));
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
}

inline void test_tree_map()
{
	const core::uint32 variation = 100;
	core::uint32 size = 2 * (core::randomize() % variation + 1);
	core::uint32 balance = 0;
	core::TreeMap<core::String, core::uint32> map;
	map.page_size(core::randomize() % variation + 1);

	// empty
	core::verify(!map.head().next());
	core::verify(!map.tail().prev());
	core::verify(map.page_size());
	core::verify(!map.size());
	core::verify(map.is_empty());
	core::verify(!map.find(core::String()).next());
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
			core::verify(core::is(map.lookup(key)));
			core::verify(i == map.lookup(key));
		}
	}

	// put/erase multi-value
	core::String key(core::randomize() % size);
	core::uint32 value = core::is(map.lookup(key))
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
		core::verify(core::is(map.lookup(key)));
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

struct IntString
{
	core::int32 i;
	core::String st;
};

inline void test_int_cast_index()
{
	const core::uint32 variation = 100;
	core::uint32 size = 2 * (core::randomize() % variation + 1);
	core::uint32 scope = 1 + core::randomize() % 20;

	typedef core::CastIndex<core::int32> IntIndex;

	core::Seq<IntString, IntIndex> seq;
	core::List<IntString, IntIndex> list(0x5);
	core::HashSet<IntString, IntIndex> hash(0x5);
	core::TreeSet<IntString, IntIndex> tree(0x5);

	for(core::uint32 i = 0; i < size; i++)
	{
		IntString is;
		is.i = core::randomize() % scope;
		is.st = core::String(core::randomize() % scope);
		seq.put(is);
		list.put(is);
		hash.put(is);
		core::verify(hash.lookup(is)->i == is.i);
		tree.put(is);
	}

	core::int32 prev = 0;
	core::Seq<IntString, IntIndex>::Iterator seq_it = seq.head();
	while(seq_it.next())
	{
		core::int32& next = (core::int32&)seq_it.value();
		core::verify(prev <= next);
		prev = next;
	}

	prev = 0;
	core::List<IntString, IntIndex>::Iterator list_it = list.head();
	while(list_it.next())
	{
		core::int32& next = (core::int32&)list_it.value();
		core::verify(prev <= next);
		prev = next;
	}

	prev = 0;
	core::TreeSet<IntString, IntIndex>::Sort tree_it = tree.sort();
	while(tree_it.next())
	{
		core::int32& next = (core::int32&)tree_it.value();
		core::verify(prev <= next);
		prev = next;
	}
}

struct StringInt
{
	core::String st;
	core::int32 i;
};

inline void test_string_cast_index()
{
	const core::uint32 variation = 100;
	core::uint32 size = 2 * (core::randomize() % variation + 1);
	core::uint32 scope = 1 + core::randomize() % 20;

	typedef core::CastIndex<core::String> StringIndex;

	core::Seq<StringInt, StringIndex> seq;
	core::List<StringInt, StringIndex> list(0x5);
	core::HashSet<StringInt, StringIndex> hash(0x5);
	core::TreeSet<StringInt, StringIndex> tree(0x5);

	for(core::uint32 i = 0; i < size; i++)
	{
		StringInt si;
		si.i = core::randomize() % scope;
		si.st = core::String(core::randomize() % scope);
		seq.put(si);
		list.put(si);
		hash.put(si);
		core::verify(hash.lookup(si)->st.equal(si.st));
		tree.put(si);
	}

	core::String prev;
	core::Seq<StringInt, StringIndex>::Iterator seq_it = seq.head();
	while(seq_it.next())
	{
		core::String& next = (core::String&)seq_it.value();
		core::verify(0 >= prev.compare(next));
		prev = next;
	}

	prev = core::String::nil();
	core::List<StringInt, StringIndex>::Iterator list_it = list.head();
	while(list_it.next())
	{
		core::String& next = (core::String&)list_it.value();
		core::verify(0 >= prev.compare(next));
		prev = next;
	}

	prev = core::String::nil();
	core::TreeSet<StringInt, StringIndex>::Sort tree_it = tree.sort();
	while(tree_it.next())
	{
		core::String& next = (core::String&)tree_it.value();
		core::verify(0 >= prev.compare(next));
		prev = next;
	}
}

inline void test_collection()
{
	core::Seq<core::String> seq;
	core::List<core::String> list;
	core::Queue<core::String> queue;
	core::HashSet<core::String> hash_set;
	core::HashMap<core::String, core::String> hash_map;
	core::TreeSet<core::String> tree_set;
	core::TreeMap<core::String, core::String> tree_map;

	core::Seq<core::String>::share shared_seq = seq;
	core::List<core::String>::share shared_list = list;
	core::Queue<core::String>::share shared_queue = queue;
	core::HashSet<core::String>::share shared_hash_set = hash_set;
	core::HashMap<core::String, core::String>::share shared_hash_map = hash_map;
	core::TreeSet<core::String>::share shared_tree_set = tree_set;
	core::TreeMap<core::String, core::String>::share shared_tree_map = tree_map;

	core::Seq<core::String>::manage managed_seq;
	core::List<core::String>::manage managed_list;
	core::Queue<core::String>::manage managed_queue;
	core::HashSet<core::String>::manage managed_hash_set;
	core::HashMap<core::String, core::String>::manage managed_hash_map;
	core::TreeSet<core::String>::manage managed_tree_set;
	core::TreeMap<core::String, core::String>::manage managed_tree_map;
}

inline void test_collection_acquire()
{
	core::List<core::String> pager(0x2);
	core::Seq<core::String::share> seq;
	core::List<core::String::share> list(0x2);
	core::HashSet<core::String::share> hash_set(0x2);
	core::HashMap<core::String, core::String::share> hash_map(0x2);
	core::TreeSet<core::String::share> tree_set(0x2);
	core::TreeMap<core::String, core::String::share> tree_map(0x2);

	for(core::uint32 i = 0; i < 100; i++)
	{
		core::String st(i);
		seq.acquire(st, pager);
		list.acquire(st, pager);
		hash_set.acquire(st, pager);
		hash_map.acquire(st, pager);
		tree_set.acquire(st, pager);
		tree_map.acquire(st, pager);
	}
}

inline void test_core()
{
	test_share();
	test_manage();
	test_string();
	test_pager();
	test_exception();
	test_acquire();
	for(core::uint32 i = 0; i < 1; i++)
	{
		test_seq();
		test_list();
		test_queue();
		test_hash_set();
		test_hash_map();
		test_tree_set();
		test_tree_map();
		test_int_cast_index();
		test_string_cast_index();
	}
	test_collection();
	test_collection_acquire();
}

#endif // TEST_CORE_HPP
