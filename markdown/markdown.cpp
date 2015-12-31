
#include <QApplication>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QVector>
#include <QTime>
#include <QtGlobal>

const double pi = 4.0 * std::atan(1.0);

#include <math.h>

enum class Mode { None, Line, List, Header, Table };

struct Context
{
	Mode mode;
	int indent;
	int header;
};

class ContextStack : public QVector<Context>
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

void convert(const QString& md)
{
	QString html = md;
	int index = md.lastIndexOf('.');
	if (index != -1)
		html = md.left(index);
	html += ".html";

	QFile md_file(md);
	md_file.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream in(&md_file);

	QFile html_file(md);
	html_file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&html_file);

	ContextStack stack;
	stack.push(Mode::Line);

	while (!in.atEnd())
	{
		char ch;
		in >> ch;
		switch (ch)
		{
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
			default:
				break;
			}
			out << ch;
		}
	}

	md_file.close();
	html_file.close();
}

int main(int argc, char* argv[])
{
	int rc = 0;
	QApplication app(argc, argv);
	if (argc != 2)
	{
		qCritical("expected: markdown <file>.md");
		rc = 1;
	}
	else
	{
		convert(argv[1]);
	}
	app.exit();
	return rc;
}
