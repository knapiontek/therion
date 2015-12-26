#ifndef PAINT_HPP
#define PAINT_HPP

#include <QTextStream>
#include <QFile>

#include <math.h>

const double pi = 3.14159265359;

struct Point2D
{
	double x, y;

	Point2D()
	{
		x = 0.0;
		y = 0.0;
	}
	Point2D(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	Point2D(const Point2D& p)
	{
		x = p.x;
		y = p.y;
	}
	static Point2D polar(double radius, double angle)
	{
		return Point2D(radius * ::cos(angle), radius * ::sin(angle));
	}
	friend Point2D operator+(const Point2D& p1, const Point2D& p2)
	{
		return Point2D(p1.x + p2.x, p1.y + p2.y);
	}
	friend Point2D operator-(const Point2D& p1, const Point2D& p2)
	{
		return Point2D(p1.x - p2.x, p1.y - p2.y);
	}
	friend Point2D operator*(double value, const Point2D& p)
	{
		return Point2D(value * p.x, value * p.y);
	}
	friend Point2D operator/(const Point2D& p, double value)
	{
		return Point2D(p.x / value, p.y / value);
	}
};

class Paint
{
public:
	enum EStyle
	{
		NONE = 0x0001,
		FILL = 0x0002,
		ARROW = 0x0004,
		DARROW = 0x0008,
		THICK = 0x0010,
		DASH = 0x0020,
		RED = 0x0040,
		LEFT = 0x0080,
		RIGHT = 0x0100,
		ABOVE = 0x0200,
		BELOW = 0x0400,
		TEXT = 0x0800
	};
	typedef int Style;
public:
	Paint(const QString& filename)
	{
		this->filename = filename;
		tex += "\\begin{tikzpicture}";
	}
	~Paint()
	{
		tex += "\n\\end{tikzpicture}";

		QFile tex_file(QString("figure/%1.tex").arg(filename));
		tex_file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream tex_stream(&tex_file);
		tex_stream << tex;
		tex_file.close();
	}
	void dot(const Point2D& p, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) circle(0.07);")
			.arg(style_tex(style | FILL))
			.arg(p.x)
			.arg(p.y);
	}
	void line(const Point2D& p1, const Point2D& p2, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) -- (%4,%5);")
			.arg(style_tex(style))
			.arg(p1.x)
			.arg(p1.y)
			.arg(p2.x)
			.arg(p2.y);
	}
	void path(const Point2D& p1, const Point2D& p2, const Point2D& p3, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) -- (%4,%5) -- (%6,%7);")
			.arg(style_tex(style))
			.arg(p1.x)
			.arg(p1.y)
			.arg(p2.x)
			.arg(p2.y)
			.arg(p3.x)
			.arg(p3.y);
	}
	void path(const Point2D& p1, const Point2D& p2, const Point2D& p3, const Point2D& p4, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) -- (%4,%5) -- (%6,%7) -- (%8,%9);")
			.arg(style_tex(style))
			.arg(p1.x)
			.arg(p1.y)
			.arg(p2.x)
			.arg(p2.y)
			.arg(p3.x)
			.arg(p3.y)
			.arg(p4.x)
			.arg(p4.y);
	}
	void arc(const Point2D& p, double radius, double angle0, double angle1, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) arc (%4:%5:%6);")
			.arg(style_tex(style))
			.arg(p.x)
			.arg(p.y)
			.arg(180/pi*angle0)
			.arg(180/pi*angle1)
			.arg(radius);
	}
	void text(const QString text, const Point2D& p, Style style = NONE)
	{
		tex += QString("\n\\node%1 at (%2,%3) {%4};")
			.arg(style_tex(style))
			.arg(p.x)
			.arg(p.y)
			.arg(text);
	}
private:
	QString style_tex(Style style)
	{
		QString name;

		if(FILL & style)
			name.append("fill");

		if(ARROW & style)
			name.append(",->");
		else if(DARROW & style)
			name.append(",<->");

		if(THICK & style)
			name.append(",thick");

		if(DASH & style)
			name.append(",dashed");

		if(RED & style)
			name.append(",red");

		if(ABOVE & style)
			name.append(",above");
		else if(BELOW & style)
			name.append(",below");
		else if(LEFT & style)
			name.append(",left");
		else if(RIGHT & style)
			name.append(",right");

		if((ABOVE | BELOW) & style)
		{
			if(LEFT & style)
				name.append(" left");
			else if(RIGHT & style)
				name.append(" right");
		}

		if(!name.isEmpty())
		{
			if(name.startsWith(','))
				name.remove(0, 1);
			name.prepend('[');
			name.append(']');
		}

		return name;
	}
private:
	QString filename;
	QString tex;
};

#endif // PAINT_HPP
