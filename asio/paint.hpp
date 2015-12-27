
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
		GREEN = 0x0080,
		BLUE = 0x0100,
		LEFT = 0x0200,
		RIGHT = 0x0400,
		ABOVE = 0x0800,
		BELOW = 0x1000,
	};
	typedef int Style;
public:
	Paint(const QString& filename)
	{
		this->filename = filename;

		svg += "<?xml version='1.0' encoding='UTF-8' standalone='no'?>"
			"\n<svg xmlns:svg='http://www.w3.org/2000/svg' xmlns='http://www.w3.org/2000/svg'>";
	}
	~Paint()
	{
		svg += "\n</svg>\n";

		QFile svg_file(filename + ".svg");
		svg_file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream svg_stream(&svg_file);
		svg_stream << svg;
		svg_file.close();
	}
	void dot(const Point2D& p, Style style = NONE)
	{
		svg += QString("\n<circle%1 (%2,%3) circle(0.05)/>")
			.arg(style_svg(style | FILL))
			.arg(p.x)
			.arg(p.y);
	}
	void line(const Point2D& p1, const Point2D& p2, Style style = NONE)
	{
		svg += QString("\n<path%1 d='M %2,%3 L %4,%5'/>")
			.arg(style_svg(style))
			.arg(p1.x)
			.arg(p1.y)
			.arg(p2.x)
			.arg(p2.y);
	}
	void path(const QList<Point2D>& coord_list, Style style = NONE)
	{
		QString points;
		QString format[2] = { " L %1,%2", "M %1,%2" };
		for(const Point2D& p : coord_list)
		{
			points += QString(format[points.isEmpty()])
				.arg(p.x)
				.arg(p.y);
		}
		svg += QString("\n<path%1 d='%2 Z'/>")
			.arg(style_svg(style))
			.arg(points);
	}
	void spline(const QList<Point2D>& coord_list, Style style = NONE)
	{
		QString points;
		QString format[2] = { " L %1,%2", "M %1,%2" };
		for(const Point2D& p : coord_list)
		{
			points += QString(format[points.isEmpty()])
				.arg(p.x)
				.arg(p.y);
		}
		svg += QString("\n<path%1 d='%2 Z'/>")
			.arg(style_svg(style))
			.arg(points);
	}
	void arc(const Point2D& p, double radius, double angle0, double angle1, Style style = NONE)
	{
		svg += QString("\n<circle%1 (%2,%3) arc (%4:%5:%6)/>")
			.arg(style_svg(style))
			.arg(p.x)
			.arg(p.y)
			.arg(180/pi*angle0)
			.arg(180/pi*angle1)
			.arg(radius);
	}
	void text(const QString text, const Point2D& p, Style style = NONE)
	{
		svg += QString("\n<text%1 at (%2,%3) {%4}>")
			.arg(style_svg(style))
			.arg(p.x)
			.arg(p.y)
			.arg(text);
	}
private:
	QString style_svg(Style style)
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
		else if(GREEN & style)
			name.append(",green");
		else if(BLUE & style)
			name.append(",blue");

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
	QString svg;
	const int color_size = 30;
};
