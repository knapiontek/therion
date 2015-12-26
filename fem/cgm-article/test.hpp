
namespace test {

	void steepest_descent()
	{
		sp::size = 2;

		sp::Vector B, R;
		B[0] = 2.0;
		B[1] = -8.0;
		R[0] = 2.0;
		R[1] = -2.0;

		sp::Matrix mx;
		mx.set(0, 0, 3.0);
		mx.set(0, 1, 2.0);
		mx.set(1, 0, 2.0);
		mx.set(1, 1, 6.0);

		sp::SymetryMatrix A = mx;

		sp::Vector x;
		x[0] = -0.5;
		x[1] = 1.2;

		for(int i = 0; i < 20; i++)
		{
			auto r = A * x;
			r -= B;
			r *= -1.0; // r := B - A * x
			double a = (r * r) / (r * A * r);
			r *= a;
			x += r; // x := x + a * r
		}

		Q_ASSERT(sp::Vector::norm2(x, R) < 0.000001);
	}

	void conjagate_gradients()
	{
		sp::size = 2;

		sp::Vector B, R;
		B[0] = 2.0;
		B[1] = -8.0;
		R[0] = 2.0;
		R[1] = -2.0;

		sp::Matrix mx;
		mx.set(0, 0, 3.0);
		mx.set(0, 1, 2.0);
		mx.set(1, 0, 2.0);
		mx.set(1, 1, 6.0);

		sp::SymetryMatrix A = mx;

		sp::Vector x;
		x[0] = -0.5;
		x[1] = 1.2;

		auto r0 = A * x;
		r0 -= B;
		r0 *= -1.0; // r0 := B - A * x
		auto p = r0;

		for(int i = 0; i < 20; i++)
		{
			double a = (r0 * r0) / (p * A * p);

			auto ap = p;
			ap *= a;
			x += ap; // x := x + a * p

			auto aAp = A * p;
			aAp *= a;
			auto r1 = r0;
			r1 -= aAp; // r1 := r0 - a * A * p

			double b = (r1 * r1) / (r0 * r0);

			p *= b;
			p += r1; // p := r1 + b * p

			r0 = r1;
		}

		Q_ASSERT(sp::Vector::norm2(x, R) < 0.000001);
	}

	void intersection()
	{
		Paint paint("figure-intersection.tex");

		Point2D b1(0.0, 0.0), e1(4.0, 4.0), b2(0.0, 4.0), e2(6.0, 0.0);

		paint.line(b1, e1);
		paint.line(b2, e2);

		paint.text("$b_1$", b1, Paint::LEFT);
		paint.text("$e_1$", e1, Paint::RIGHT);
		paint.text("$b_2$", b2, Paint::LEFT);
		paint.text("$e_2$", e2, Paint::RIGHT);

		Point2D v = e1 - b1;
		double n = v.norm();
		Point2D u = v / n;

		Point2D arm1 = b2 - b1;
		Point2D arm2 = e2 - b1;

		double cos1 = arm1 * u;
		double cos2 = arm2 * u;

		double sin1 = arm1 ^ u;
		double sin2 = arm2 ^ u;

		if(sin1 * sin2 <= 0)
		{
			double sin = sin1 - sin2;
			if(sin != 0)
			{
				double ratio = (cos2 - cos1) * sin1 / sin; // Congruent Triangles
				double scale = cos1 + ratio;
				if(0 < scale && scale < n)
				{
					Point2D result = b1 + scale * u;

					Point2D a1 = b1 + cos1 * u;
					Point2D a2 = b1 + cos2 * u;
					paint.dot(a1);
					paint.dot(a2);
					paint.text("$a_1$", a1, Paint::RIGHT | Paint::BELOW);
					paint.text("$a_2$", a2, Paint::LEFT | Paint::ABOVE);
					paint.line(b2, a1, Paint::DASH);
					paint.line(e2, a2, Paint::DASH);

					paint.dot(result);
				}
			}
		}
	}

	void store_elements(QString filename, const Point3D point_list[], const data::Element element_list[], int element_size)
	{
		QVector<clip::Element> elements = clip::make(point_list, element_list, element_size, clip::no_rotate);

		Paint paint(filename);

		for(auto& e : elements)
		{
			if(e.pt1 != e.pt2)
			{
				paint.line(e.pt1, e.pt2);
			}
		}
	}

	void clip_triangles()
	{
		const Point3D point_list1[]
		{
			{ 0.0, 0.0, 0.0 }, // 0
			{ 2.0, 0.0, 0.0 }, // 1
			{ 1.0, 2.0, 0.0 }, // 2
			{ 1.0, 1.0, 2.0 }, // 3
			{ 3.0, 1.0, 2.0 }, // 4
			{ 2.0, 3.0, 2.0 }, // 5
			{ 2.0, 2.0, 1.0 }, // 6
			{ 4.0, 2.0, 1.0 }, // 7
			{ 3.0, 4.0, 1.0 }, // 8
			{ 1.5, 2.5, 3.0 }, // 9
			{ 2.5, 2.5, 3.0 }, // 10
			{ 2.0, 3.5, 3.0 } // 11
		};
		const data::Element element_list1[]
		{
			{ 0, 1 },
			{ 1, 2 },
			{ 0, 2 },
			{ 3, 4 },
			{ 4, 5 },
			{ 3, 5 },
			{ 6, 7 },
			{ 7, 8 },
			{ 6, 8 },
			{ 9, 10 },
			{ 10, 11 },
			{ 9, 11 }
		};
		store_elements("figure-triangle1.tex", point_list1, element_list1, sizeof(element_list1) / sizeof(data::Element));

		const Point3D point_list2[]
		{
			{ 0.0, 0.0, 2.0 }, // 0
			{ 2.0, 0.0, 2.0 }, // 1
			{ 2.0, 2.0, 2.0 }, // 2
			{ 0.0, 2.0, 2.0 }, // 3
			{ 1.0, 0.3, 1.0 }, // 4
			{ 3.0, 0.3, 1.0 }, // 5
			{ 3.0, 2.3, 1.0 }, // 6
			{ 1.0, 2.3, 1.0 } // 7
		};
		const data::Element element_list2[]
		{
			{ 0, 1 },
			{ 1, 2 },
			{ 2, 3 },
			{ 3, 0 },
			{ 0, 2 },
			{ 4, 5 },
			{ 5, 6 },
			{ 6, 7 },
			{ 7, 4 },
			{ 4, 6 }
		};
		store_elements("figure-triangle2.tex", point_list2, element_list2, sizeof(element_list2) / sizeof(data::Element));

		const Point3D point_list3[]
		{
			{ 0.1, 0.7, 2.0 }, // 0
			{ 3.0, 0.0, 2.0 }, // 1
			{ 3.0, 2.0, 2.0 }, // 2
			{ 0.0, 0.5, 1.0 }, // 3
			{ 0.0, 1.5, 1.0 }, // 4
			{ 0.5, 0.5, 1.0 }, // 5
			{ 0.5, 1.5, 1.0 }, // 6
			{ 1.0, 0.7, 1.0 }, // 7
			{ 1.0, 1.5, 1.0 }, // 8
			{ 1.5, 0.9, 1.0 }, // 9
			{ 1.5, 1.9, 1.0 }, // 10
			{ 2.0, 0.0, 1.0 }, // 11
			{ 2.0, 1.0, 1.0 }, // 12
			{ 2.5, 0.5, 1.0 }, // 13
			{ 2.5, 1.5, 1.0 }, // 14
			{ 2.0, 2.0, 1.0 }, // 15
			{ 2.8, 2.2, 2.1 }, // 16
		};
		const data::Element element_list3[]
		{
			{ 3, 4 },
			{ 5, 6 },
			{ 7, 8 },
			{ 9, 10 },
			{ 11, 12 },
			{ 13, 14 },
			{ 1, 15 },
			{ 1, 16 },
			{ 0, 1 },
			{ 1, 2 },
			{ 2, 0 },
		};
		store_elements("figure-triangle3.tex", point_list3, element_list3, sizeof(element_list3) / sizeof(data::Element));
	}
}
