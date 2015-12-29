
void draw_thread_switch_context()
{
	Paint paint("thread_switch_context");

	Point2D b1(0, 20), e1(100, 20);
	Point2D b2(0, 40), e2(100, 40);

	paint.line(b1, e1, Paint::RED);
	paint.line(b2, e2, Paint::BLUE);

	QList<Point2D> coord_list
	{
		Point2D(100, 100),
		Point2D(200, 100),
		Point2D(100, 200)
	};
	paint.path(coord_list, Paint::GREEN);
}
