
#ifndef LANGEN_MANGLE_HPP
#define LANGEN_MANGLE_HPP

#include "core/core/string.hpp"
#include "core/core/seq.hpp"
#include "core/core/hashmap.hpp"

class KeywordSeq
{
public:
	static bool contain(core::String& keyword)
	{
		static KeywordSeq keyword_seq;
		return core::uint32_nil != keyword_seq.the_seq.search(keyword);
	}
private:
	KeywordSeq()
	{
		static const core::String keywords[] = {
			"and",
			"asm",
			"auto",
			"bitand",
			"bitor",
			"bool",
			"break",
			"case",
			"catch",
			"char",
			"class",
			"const",
			"const_cast",
			"continue",
			"default",
			"delete",
			"do",
			"double",
			"dynamic_cast",
			"else",
			"enum",
			"explicit",
			"extern",
			"false",
			"float",
			"for",
			"friend",
			"goto",
			"if",
			"inline",
			"int",
			"long",
			"mutable",
			"namespace",
			"new",
			"not",
			"not_eq",
			"operator",
			"or",
			"or_eq",
			"private",
			"protected",
			"public",
			"register",
			"reinterpret_cast",
			"return",
			"short",
			"signed",
			"sizeof",
			"static",
			"static_cast",
			"struct",
			"switch",
			"template",
			"this",
			"throw",
			"true",
			"try",
			"typedef",
			"typeid",
			"typename",
			"union",
			"unsigned",
			"using",
			"virtual",
			"void",
			"volatile",
			"wchar_t",
			"while",
			"xor",
			"xor_eq"
		};
		core::uint32 size = sizeof(keywords) / sizeof(core::String);
		the_seq.size(size);
		for(core::uint32 i = 0; i < size; i++)
			the_seq.at(i) = keywords[i];
	}
private:
	core::Seq<core::String> the_seq;
};

struct Mangle
{
	static bool unhex(core::String& arg)
	{
		core::uint32 pos = 0;
		core::uint32 last = 0;
		core::String result;
		core::String::Iterator it = arg.head();
		while(it.next())
		{
			if('$' == it.value())
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				if(!it.next())
					return false;
				core::uint8 byte1 = it.value();
				if('n' == byte1)
					byte1 = '\n';
				else if('t' == byte1)
					byte1 = '\t';
				else if('s' == byte1)
					byte1 = ' ';
				else if('$' != byte1)
				{
					if(!it.next())
						return false;
					core::uint8 byte2 = it.value();
					if(!xdigit(byte1) || !xdigit(byte2))
						return false;
					byte1 = 0x10 * byte1 + byte2;
				}
				result.copy_in(pos, &byte1, sizeof(byte1));
				last = it.position() + 1;
			}
		}
		if(pos)
		{
			core::assert(last <= arg.size());
			result.copy_in(pos, arg.data() + last, arg.size() - last);
			result.size(pos);
			arg = result;
		}
		return true;
	}
	static core::String hex(core::uint8 arg)
	{
		if('$' == arg)
			return "$$";
		else if('\n' == arg)
			return "$n";
		else if('\t' == arg)
			return "$t";
		else if(' ' == arg)
			return "$s";
		else if(::isgraph(arg))
			return core::String().attach(arg);
		else
		{
			core::uint8 hex[] = { '$', byte2hex(arg / 0x10), byte2hex(arg % 0x10) };
			return core::String(hex, sizeof(hex));
		}
	}
	static core::String hex(core::String& arg)
	{
		core::uint32 pos = 0;
		core::uint32 last = 0;
		core::String result;
		core::String::Iterator it = arg.head();
		while(it.next())
		{
			core::uint8 byte = it.value();
			if('$' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "$$");
			}
			else if('\n' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "$n");
			}
			else if('\t' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "$t");
			}
			else if(' ' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "$s");
			}
			else if(!::isgraph(byte))
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				core::uint8 hex[] = { '$', byte2hex(byte / 0x10), byte2hex(byte % 0x10) };
				result.copy_in(pos, hex, sizeof(hex));
			}
		}
		if(pos)
		{
			core::assert(last <= arg.size());
			result.copy_in(pos, arg.data() + last, arg.size() - last);
			result.size(pos);
			return result;
		}
		return arg;
	}
	static core::String type2var(core::String& arg)
	{
		core::uint8 upper = 0;
		core::uint32 pos = 0;
		core::uint32 last = 0;
		core::String result;
		core::String::Iterator it = arg.head();
		if(it.next())
		{
			core::uint8 byte = it.value();
			if(::isupper(byte))
			{
				core::uint8 lower = ::tolower(byte);
				result.copy_in(pos, &lower, sizeof(lower));
				last = 1;
			}
		}
		while(it.next())
		{
			core::uint8 byte = it.value();
			if(::isupper(byte))
				upper = byte;
			else if(::islower(byte) && upper)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last - 1);
				last = it.position();
				core::uint8 lower[] = { '_', ::tolower(upper) };
				result.copy_in(pos, lower, sizeof(lower));
				upper = 0;
			}
		}
		if(pos)
		{
			core::assert(last <= arg.size());
			result.copy_in(pos, arg.data() + last, arg.size() - last);
			result.size(pos);
			if(KeywordSeq::contain(result))
				result.prepend("var_");
			return result;
		}
		if(KeywordSeq::contain(arg))
			arg.prepend("var_");
		return arg;
	}
	static core::String value2var(core::String& arg)
	{
		core::uint32 pos = 0;
		core::uint32 last = 0;
		core::String result;
		core::String::Iterator it = arg.head();
		while(it.next())
		{
			core::uint8 byte = it.value();
			if(core::nil(result) && ::isdigit(byte))
				result.copy_in(pos, "var_");
			else if(!::isalnum(byte))
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				core::uint8 hex[] = { 'x', byte2hex(byte / 0x10), byte2hex(byte % 0x10) };
				result.copy_in(pos, hex, sizeof(hex));
			}
		}
		if(pos)
		{
			core::assert(last <= arg.size());
			result.copy_in(pos, arg.data() + last, arg.size() - last);
			result.size(pos);
			if(KeywordSeq::contain(result))
				result.prepend("var_");
			return result;
		}
		if(KeywordSeq::contain(arg))
			arg.prepend("var_");
		return arg;
	}
	static core::String value2constant(core::String& arg)
	{
		bool encode = false;
		core::uint32 pos = 0;
		core::uint32 last = 0;
		core::String result;
		core::String::Iterator it = arg.head();
		while(it.next())
		{
			core::uint8 byte = it.value();
			if('\\' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "\\\\");
			}
			else if('\"' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "\\\"");
			}
			else if('\n' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "\\n");
			}
			else if('\t' == byte)
			{
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				result.copy_in(pos, "\\t");
			}
			else if(!::isprint(byte))
			{
				encode = true;
				result.copy_in(pos, arg.data() + last, it.position() - last);
				last = it.position() + 1;
				core::uint8 hex[] = { '\\', 'x', byte2hex(byte / 0x10), byte2hex(byte % 0x10) };
				result.copy_in(pos, hex, sizeof(hex));
			}
			else if(encode && ::isxdigit(byte))
			{
				encode = false;
				result.copy_in(pos, "\"\"");
			}
		}
		if(pos)
		{
			core::assert(last <= arg.size());
			result.copy_in(pos, arg.data() + last, arg.size() - last);
			result.size(pos);
			return result;
		}
		return arg;
	}
	static core::uint8 byte2hex(core::uint8 byte)
	{
		static const core::uint8 byte2hex[] = {
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'A', 'B', 'C', 'D', 'E', 'F'
		};
		core::assert(byte < sizeof(byte2hex));
		return byte2hex[byte];
	}
	static bool xdigit(core::uint8& arg)
	{
		static const core::uint8 hex2byte[] = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
			-1, -1, -1, -1, -1, -1, -1,
			0xA, 0xB, 0xC, 0xD, 0xE, 0xF
		};
		core::uint8 dt = arg - '0';
		if(dt < sizeof(hex2byte))
		{
			arg = hex2byte[dt];
			return -1 != arg;
		}
		return false;
	}
};

#endif // LANGEN_MANGLE_HPP
