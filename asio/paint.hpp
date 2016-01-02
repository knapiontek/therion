
class Paint
{
public:
	typedef int Style;
	enum
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
public:
	Paint(const QString& filename)
	{
		this->filename = filename;

		svg += "<?xml version='1.0' encoding='UTF-8' standalone='no'?>"
			"\n<svg version='1.1' xmlns='http://www.w3.org/2000/svg'>"
			"\n<defs>"
			"\n<marker id='arrow-end' markerWidth='13' markerHeight='13' refX='12' refY='4' orient='auto'>"
			"\n<path d='M0,0 L0,8 L12,4 Z'/>"
			"\n</marker>"
			"\n<marker id='arrow-start' markerWidth='13' markerHeight='13' refX='0' refY='4' orient='auto'>"
			"\n<path d='M12,0 L12,8 L0,4 Z'/>"
			"\n</marker>"
			"\n</defs>";
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
	void line(const Point2D& p1, const Point2D& p2, Style style)
	{
		svg += QString("\n<path%1 d='M%2,%3 L%4,%5'/>")
			.arg(style_svg(style))
			.arg(pos_x(p1.x))
			.arg(pos_y(p1.y))
			.arg(pos_x(p2.x))
			.arg(pos_y(p2.y));
	}
	void path(const QList<Point2D>& coord_list, Style style)
	{
		QString points;
		QString format[2] = { " L%1,%2", "M%1,%2" };
		for(const Point2D& p : coord_list)
		{
			points += QString(format[points.isEmpty()])
				.arg(pos_x(p.x))
				.arg(pos_y(p.y));
		}
		svg += QString("\n<path%1 d='%2'/>")
			.arg(style_svg(style))
			.arg(points);
	}
	void text(const QString text, const Point2D& p, Style style)
	{
		svg += QString("\n<text%1 at (%2,%3) {%4}>")
			.arg(style_svg(style))
			.arg(pos_x(p.x))
			.arg(pos_y(p.y))
			.arg(text);
	}
private:
	QString style_svg(Style style)
	{
		QString name = " fill='none'";

		if (!(style & (red | green | blue)))
			style |= black;

		const struct
		{
			Style style;
			QString value;
		}
		format_seq[]
		{
			{ black, " stroke='black'" },
			{ red, " stroke='red'" },
			{ green, " stroke='green'" },
			{ blue, " stroke='blue'" },
			{ north, "" },
			{ south, "" },
			{ west, "" },
			{ east, "" },
			{ arrow1, " marker-end='url(#arrow-end)'" },
			{ arrow2, " marker-start='url(#arrow-start)' marker-end='url(#arrow-end)'" },
			{ thick, " stroke-width='2'" },
			{ dash, ""}
		};

		for(const auto& format : format_seq)
		{
			if(format.style & style)
				name += format.value;
		}

		return name;
	}
	double pos_x(double x)
	{
		return scale * x;
	}
	double pos_y(double y)
	{
		return scale * (region.y - y);
	}
private:
	QString filename;
	QString svg;
	Point2D region = { 100, 100 };
	double scale = 6;
};
