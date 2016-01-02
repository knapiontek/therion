
void draw_thread_switch_context(Paint& paint, double step, double y)
{
	Point2D b1(10, y + 30), e1(90, y + 30);
	Point2D b2(10, y + 60), e2(90, y + 60);

	paint.line(b1, e1, Paint::arrow1);
	paint.line(b2, e2, Paint::arrow1);

	double half = step / 2;
	QList<Point2D> coord_list;
	for(double i = 15; i < 85 - step; i += step)
	{
		QList<Point2D> cycle_list
		{
			Point2D(i, y + 60),
			Point2D(i, y + 30),
			Point2D(i + half, y + 30),
			Point2D(i + half, y + 60),
		};
		coord_list += cycle_list;
	}
	paint.path(coord_list, Paint::red | Paint::thick);
}

void draw_thread_switch_context()
{
	Paint paint("thread_switch_context");
	draw_thread_switch_context(paint, 15, 30);
	draw_thread_switch_context(paint, 2, -10);
}
