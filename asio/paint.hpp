
class Paint
{
public:
	enum StyleEnum
	{
		black = 1 << 0,
		red = 1 << 1,
		green = 1 << 2,
		blue = 1 << 3,
		north = 1 << 4,
		south = 1 << 5,
		west = 1 << 6,
		east = 1 << 7,
		arrow1 = 1 << 8,
		arrow2 = 1 << 9,
		thick = 1 << 10,
		dash = 1 << 11
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
	void dot(const Point2D& p, Style style)
	{
		svg += QString("\n<circle%1 (%2,%3) circle(0.05)/>")
			.arg(style_svg(style))
			.arg(p.x)
			.arg(p.y);
	}
	void line(const Point2D& p1, const Point2D& p2, Style style)
	{
		svg += QString("\n<path%1 d='M %2,%3 L %4,%5'/>")
			.arg(style_svg(style))
			.arg(p1.x)
			.arg(p1.y)
			.arg(p2.x)
			.arg(p2.y);
	}
	void path(const QList<Point2D>& coord_list, Style style)
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
	void spline(const QList<Point2D>& coord_list, Style style)
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
	void arc(const Point2D& p, double radius, double angle0, double angle1, Style style)
	{
		svg += QString("\n<circle%1 (%2,%3) arc (%4:%5:%6)/>")
			.arg(style_svg(style))
			.arg(p.x)
			.arg(p.y)
			.arg(180/pi*angle0)
			.arg(180/pi*angle1)
			.arg(radius);
	}
	void text(const QString text, const Point2D& p, Style style)
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

		struct
		{
			Style style;
			QString value;
		}
		formats[]
		{
			{ black, " stroke='black'"},
			{ red, " stroke='red'"},
			{ green, " stroke='green'"},
			{ blue, " stroke='blue'"},
			{ north, ""},
			{ south, ""},
			{ west, ""},
			{ east, ""},
			{ arrow1, ""},
			{ arrow2, ""},
			{ thick, ""},
			{ dash, ""}
		};

		for(const auto& format : formats)
		{
			if(format.style & style)
				name += format.value;
		}

		return name;
	}
private:
	QString filename;
	QString svg;
	const int color_size = 30;
};
