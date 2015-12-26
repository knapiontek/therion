
void draw_thread_switch_context()
{
	Paint paint("thread_switch_context");

	Point2D b1(0.0, 2.0), e1(10.0, 2.0);
	Point2D b2(0.0, 0.0), e2(10.0, 0.0);

	paint.line(b1, e1);
	paint.line(b2, e2);
}
