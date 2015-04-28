#ifndef FORMAT_HPP
#define FORMAT_HPP

#include <QTextStream>
#include <QFile>

#include <math.h>

const double pi = 3.1415;

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
	Paint(const QString& filename, int width, int height, int pos_x, int pos_y)
	{
		this->filename = filename;
		this->pos_x = pos_x;
		this->pos_y = pos_y;

		tex += "\\begin{tikzpicture}";
		html += "<div class=\"container\">";
		svg += QString("\n<svg width=\"%1\" height=\"%2\">"
			"\n\t<defs>"
				"\n\t\t<marker id=\"arrow-start-black\" markerWidth=\"13\" markerHeight=\"13\" refx=\"0\" refy=\"4\" orient=\"auto\">"
					"\n\t\t\t<path d=\"M12,0 L12,8 L0,4 z\" stroke=\"black\" fill=\"black\"/>"
				"\n\t\t</marker>"
				"\n\t\t<marker id=\"arrow-end-black\" markerWidth=\"13\" markerHeight=\"13\" refx=\"12\" refy=\"4\" orient=\"auto\">"
					"\n\t\t\t<path d=\"M0,0 L0,8 L12,4 z\" stroke=\"black\" fill=\"black\"/>"
				"\n\t\t</marker>"
				"\n\t\t<marker id=\"arrow-start-red\" markerWidth=\"13\" markerHeight=\"13\" refx=\"0\" refy=\"4\" orient=\"auto\">"
					"\n\t\t\t<path d=\"M12,0 L12,8 L0,4 z\" stroke=\"red\" fill=\"red\"/>"
				"\n\t\t</marker>"
				"\n\t\t<marker id=\"arrow-end-red\" markerWidth=\"13\" markerHeight=\"13\" refx=\"12\" refy=\"4\" orient=\"auto\">"
					"\n\t\t\t<path d=\"M0,0 L0,8 L12,4 z\" stroke=\"red\" fill=\"red\"/>"
				"\n\t\t</marker>"
			"\n\t</defs>").arg(width).arg(height);
	}
	~Paint()
	{
		tex += "\n\\end{tikzpicture}";
		html += "\n</div>";
		svg += "\n</svg>";

		QFile tex_file(QString("figure/%1.tex").arg(filename));
		tex_file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream tex_stream(&tex_file);
		tex_stream << tex;
		tex_file.close();

		QFile svg_file(QString("html/%1.svg").arg(filename));
		svg_file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream svg_stream(&svg_file);
		svg_stream << html << svg;
		svg_file.close();
	}
	void dot(const Point2D& p, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) circle(0.07);")
			.arg(style_tex(style | FILL))
			.arg(p.x)
			.arg(p.y);
		svg += QString("\n\t<circle %1 cx=\"%2\" cy=\"%3\" r=\"3\"/>")
			.arg(style_svg(style | FILL))
			.arg(scalex(p.x))
			.arg(scaley(p.y));
	}
	void line(const Point2D& p1, const Point2D& p2, Style style = NONE)
	{
		tex += QString("\n\\draw%1 (%2,%3) -- (%4,%5);")
			.arg(style_tex(style))
			.arg(p1.x)
			.arg(p1.y)
			.arg(p2.x)
			.arg(p2.y);
		svg += QString("\n\t<line %1 x1=\"%2\" y1=\"%3\" x2=\"%4\" y2=\"%5\"/>")
			.arg(style_svg(style))
			.arg(scalex(p1.x))
			.arg(scaley(p1.y))
			.arg(scalex(p2.x))
			.arg(scaley(p2.y));
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
		svg += QString("\n\t<path %1 d=\"M%2 %3 L%4 %5 L%6 %7\" fill=\"none\"/>")
			.arg(style_svg(style))
			.arg(scalex(p1.x))
			.arg(scaley(p1.y))
			.arg(scalex(p2.x))
			.arg(scaley(p2.y))
			.arg(scalex(p3.x))
			.arg(scaley(p3.y));
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
		svg += QString("\n\t<path %1 d=\"M%2 %3 L%4 %5 L%6 %7 L%8 %9\" fill=\"none\"/>")
			.arg(style_svg(style))
			.arg(scalex(p1.x))
			.arg(scaley(p1.y))
			.arg(scalex(p2.x))
			.arg(scaley(p2.y))
			.arg(scalex(p3.x))
			.arg(scaley(p3.y))
			.arg(scalex(p4.x))
			.arg(scaley(p4.y));
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
		Point2D p1 = p - Point2D::polar(radius, angle0) + Point2D::polar(radius, angle1);
		svg += QString("\n\t<path fill=\"none\" %1 d=\"M%2 %3 A%4 %5 0 0 0 %6 %7\"/>")
			.arg(style_svg(style))
			.arg(scalex(p.x))
			.arg(scaley(p.y))
			.arg(scale(radius))
			.arg(scale(radius))
			.arg(scalex(p1.x))
			.arg(scaley(p1.y));
	}
	void text(const QString text, const Point2D& p, Style style = NONE)
	{
		tex += QString("\n\\node%1 at (%2,%3) {%4};")
			.arg(style_tex(style))
			.arg(p.x)
			.arg(p.y)
			.arg(text);
		svg += QString("\n\t<text %1 x=\"%2\" y=\"%3\">%4</text>")
			.arg(style_svg(style | TEXT))
			.arg(scalex(p.x, style))
			.arg(scaley(p.y, style))
			.arg(text);
	}
	void math(const QString text, const Point2D& p, Style style = NONE)
	{
		tex += QString("\n\\node%1 at (%2,%3) {%4};")
			.arg(style_tex(style))
			.arg(p.x)
			.arg(p.y)
			.arg(text);
		html += QString("\n\t<div %1 style=\"left:%2px;top:%3px;\">%4</div>")
			.arg(style_math(style))
			.arg(scalex(p.x, style))
			.arg(scaley(p.y, style))
			.arg(text);
	}
private:
	int scalex(double value, Style style = NONE)
	{
		int ivalue = pos_x + (40 * value);
		if(LEFT & style)
			ivalue -= 3;
		else if(RIGHT & style)
			ivalue += 3;
		return ivalue;
	}
	int scaley(double value, Style style = NONE)
	{
		int ivalue = pos_y - (40 * value);
		if(ABOVE & style)
			ivalue -= 3;
		else if(BELOW & style)
			ivalue += 3;
		return ivalue;
	}
	int scale(double value)
	{
		return 40 * value;
	}
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
	QString style_svg(Style style)
	{
		QString name;

		if(RED & style)
			name.append("stroke=\"red\"");
		else if(!(TEXT & style))
			name.append("stroke=\"black\"");

		if(FILL & style)
		{
			if(RED & style)
				name.append(" fill=\"red\"");
			else
				name.append(" fill=\"black\"");
		}

		if(DASH & style)
			name.append(" stroke-dasharray=\"7,7\"");

		if(RED & style)
		{
			if(ARROW & style)
				name.append(" marker-end=url(#arrow-end-red)");
			else if(DARROW & style)
				name.append(" marker-start=url(#arrow-start-red) marker-end=url(#arrow-end-red)");
		}
		else
		{
			if(ARROW & style)
				name.append(" marker-end=url(#arrow-end-black)");
			else if(DARROW & style)
				name.append(" marker-start=url(#arrow-start-black) marker-end=url(#arrow-end-black)");
		}

		if(ABOVE & style)
			;
		else if(BELOW & style)
			name.append(" baseline-shift=\"-100%\"");
		else if(TEXT & style)
			name.append(" baseline-shift=\"-40%\"");

		if(LEFT & style)
			name.append(" text-anchor=\"end\"");
		else if(RIGHT & style)
			;
		else if(TEXT & style)
			name.append(" text-anchor=\"middle\"");

		return name;
	}
	QString style_math(Style style)
	{
		QString name;

		name.append("class=\"math");

		if(ABOVE & style)
			name.append(" above_pos");
		else if(BELOW & style)
			name.append(" below_pos");
		else
			name.append(" vcenter_pos");

		if(LEFT & style)
			name.append(" left_pos");
		else if(RIGHT & style)
			name.append(" right_pos");
		else
			name.append(" hcenter_pos");

		name.append("\"");

		if(RED & style)
			name.append(" stroke=\"red\"");

		return name;
	}
private:
	QString filename;
	QString tex;
	QString html;
	QString svg;
	int pos_x, pos_y;
};

#endif // FORMAT_HPP
