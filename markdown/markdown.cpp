
#include <QApplication>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QVector>
#include <QTime>
#include <QtGlobal>

const double pi = 4.0 * std::atan(1.0);

#include <math.h>

enum class Mode { list, table };

struct Context
{
	Mode mode;
	int indent;
};

class ContextStack : public QVector<Context>
{
public:
	void push(Mode mode)
	{
		Context context { mode, 0 };
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

	QFile html_file(html);
	html_file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&html_file);

	ContextStack stack;

	while (!in.atEnd())
	{
		char ch;
		in >> ch;
		switch (ch)
		{
		case '[':
			out << "\n</table>\n<tr><td>";
			break;
		case '|':
			out << "</td><td>";
			break;
		case ']':
			out << "</td></tr>\n</table>";
			break;
		default:
			out << ch;
		}
	}

	md_file.close();
	html_file.close();
}

int main(int argc, char* argv[])
{
	int rc = 0;
	if (argc != 2)
	{
		qCritical("expected: markdown <file>.md");
		rc = 1;
	}
	else
	{
		convert(argv[1]);
	}
	return rc;
}
