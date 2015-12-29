
void draw_thread_switch_context()
{
	Paint paint("thread_switch_context");

	QList<Point2D> coord_list
	{
		Point2D(10, 30),
		Point2D(20, 30),
		Point2D(20, 60),
		Point2D(40, 60),
		Point2D(40, 30),
		Point2D(60, 30),
		Point2D(60, 60),
		Point2D(80, 60),
		Point2D(80, 30),
		Point2D(90, 30)
	};
	paint.path(coord_list, Paint::red);

	Point2D b1(10, 30), e1(90, 30);
	Point2D b2(10, 60), e2(90, 60);

	paint.line(b1, e1, Paint::arrow1);
	paint.line(b2, e2, Paint::arrow2);
}
