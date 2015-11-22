#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>

enum class Mode { None, Line, List, Header, Table };

struct Context
{
	Mode mode;
	int indent;
	int header;
};

class ContextStack : public std::vector<Context>
{
public:
	void push(Mode mode)
	{
		Context context { mode, 0, 0 };
		push_back(context);
	}
	Context pop()
	{
		Context context = back();
		pop_back();
		return context;
	}
	Context& top()
	{
		return back();
	}
};

void convert(const std::string& md)
{
	std::string html = md;
	int index = md.rfind('.');
	if (index != std::string::npos)
		html = md.substr(0, index);
	html += ".html";

	std::ifstream in(md, std::ios::in);
	std::ofstream out(html, std::ios::out);

	ContextStack stack;
	stack.push(Mode::Line);

	while (!in.eof())
	{
		char ch = in.get();
		switch (ch)
		{
		case ' ':
		case '\t':
			stack.top().indent++;
			out << ch;
			break;
		case '\n':
			if (stack.top().mode == Mode::List)
			{
				out << "\n</li>";
				stack.top().mode = Mode::Line;
			}
			if (stack.top().mode != Mode::Line)
			{
				stack.push(Mode::Line);
			}
		case '*':
			if (stack.top().mode == Mode::Line)
			{
				out << "\n<li>";
				stack.top().mode = Mode::List;
			}
			break;
		case '#':
			stack.top().header++;
			break;
		case '[':
			out << "\n</table>\n<tr>";
			break;
		case '|':
			out << "\n</td>\n<td>";
			break;
		case ']':
			out << "\n</tr>\n</table>";
			break;
		default:
			switch (stack.top().mode)
			{
			case Mode::Line:
				stack.pop();
				break;
			case Mode::Header:
				out << "\n<h" << stack.top().header << ">";
				break;
			}
			out << ch;
		}
	}
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
			throw std::invalid_argument("expected: markdown <file>.md");
		else
			convert(argv[1]);
		return 0;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
}
