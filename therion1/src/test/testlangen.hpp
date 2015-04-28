
#ifndef TEST_LANGEN_HPP
#define TEST_LANGEN_HPP

#include "core/io/output.hpp"
#include "core/core/pager.hpp"
#include "core/core/list.hpp"
#include "test/sample/match.hpp"
#include "test/sample/floatplus.hpp"
#include "test/sample/floatlist.hpp"
#include "test/sample/conflict.hpp"
#include "test/cs.hpp"

class StringOutput : public io::Output
{
public:
	core::String& content()
	{
		return the_content;
	}
private:
	void output(core::uint8* data, core::uint32 size)
	{
		the_content.append(data, size);
	}
private:
	core::String the_content;
};

inline void test_match()
{
	core::String input = cs("**+%;-*+%;-+%;<");
	StringOutput output;
	match::Decode decode;
	match::Encode encode(output);

	// put
	core::String::Iterator it = input.head();
	while(it.next() && decode.put(it.value()));
	match::Model model = decode.model();
	core::verify(model);
	encode.write(model);
	core::verify(output.content().equal(input));
}

inline void test_floatplus()
{
	core::String input = cs("12.3 %12.3 %12%.3 *12%.3 *12*.3 12%.3 -12.3 %-12+.3 %-12%+.3 *-12%+.3 *-12*+.3 -12%+.3 \0");
	StringOutput output;
	floatplus::Decode decode;
	floatplus::Encode encode(output);

	// put
	core::String::Iterator it = input.head();
	while(it.next() && decode.put(it.value()));
	floatplus::Model model = decode.model();
	core::verify(model);
	encode.write(model);
	core::verify(output.content().equal(input));
}

inline void test_floatlist()
{
	core::String input = cs("x$ $-123.4E56,7.89e1,2.E-345\t7^\0");
	StringOutput output;
	floatlist::Decode decode;
	floatlist::Encode encode(output);

	// put
	core::String::Iterator it = input.head();
	while(it.next() && decode.put(it.value()));
	floatlist::Model model = decode.model();
	core::verify(model);
	encode.write(model);
	core::verify(output.content().equal(input));

	// error
	input = cs("x$ $1.2\t7w");
	output.content().erase();
	decode.release();
	it = input.head();
	while(it.next() && decode.put(it.value()));
	core::verify('w' == it.value());
	core::verify(!decode.model());

	// acquire
	output.content().erase();
	decode.release();

	floatlist::Optional optional = decode.acquire_Optional2();
	floatlist::SignQues minus = floatlist::x2D;
	floatlist::DigitPlus digit_plus1 = decode.acquire_DigitPlus1(floatlist::var_7);
	floatlist::Decimal decimal = decode.acquire_Decimal(optional, minus, digit_plus1, optional);
	core::uint8 digits[] = { floatlist::var_7, floatlist::var_1 };
	floatlist::DigitPlus digit_plus2 = decode.acquire_String(bs(digits));
	floatlist::Float var_float = decode.acquire_Float(decimal, digit_plus2, floatlist::E, decimal);

	encode.write(var_float);
	core::verify(output.content().equal("-7.71E-7"));
}

inline void test_conflict()
{
	core::String input = cs("ins");
	StringOutput output;
	conflict::Decode decode;
	conflict::Encode encode(output);

	// put
	core::String::Iterator it = input.head();
	while(it.next() && decode.put(it.value()));
	conflict::Model model = decode.model();
	core::verify(model);
	encode.write(model);
	core::verify(output.content().equal(input));
}

inline void test_langen()
{
	test_match();
	test_floatplus();
	test_floatlist();
	//test_conflict();
}

#endif // TEST_LANGEN_HPP
