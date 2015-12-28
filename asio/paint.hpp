
class Paint
{
public:
	enum EStyle
	{
		STROKE_BLACK = 1 << 0,
		STROKE_RED = 1 << 1,
		STROKE_GREEN = 1 << 2,
		STROKE_BLUE = 1 << 3,
		FILL_BLACK = 1 << 4,
		FILL_RED = 1 << 5,
		FILL_GREEN = 1 << 6,
		FILL_BLUE = 1 << 7,
		POS_NORTH = 1 << 8,
		POS_SOUTH = 1 << 9,
		POS_WEST = 1 << 10,
		POS_EAST = 1 << 11,
		ARROW_1 = 1 << 12,
		ARROW_2 = 1 << 13,
		THICK = 1 << 14,
		DASH = 1 << 15,
		BLACK = STROKE_BLACK | FILL_BLACK,
		RED = STROKE_RED | FILL_RED,
		GREEN = STROKE_GREEN | FILL_GREEN,
		BLUE = STROKE_BLUE | FILL_BLUE
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
			{ STROKE_BLACK, " stroke='black'"},
			{ STROKE_RED, " stroke='red'"},
			{ STROKE_GREEN, " stroke='green'"},
			{ STROKE_BLUE, " stroke='blue'"},
			{ FILL_BLACK, " fill='black'"},
			{ FILL_RED, " fill='red'"},
			{ FILL_GREEN, " fill='green'"},
			{ FILL_BLUE, " fill='blue'"},
			{ POS_NORTH, ""},
			{ POS_SOUTH, ""},
			{ POS_WEST, ""},
			{ POS_EAST, ""},
			{ ARROW_1, ""},
			{ ARROW_2, ""},
			{ THICK, ""},
			{ DASH, ""}
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
