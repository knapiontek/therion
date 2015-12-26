
namespace doc
{
	double scale = 0.06;

	Matrix2D A(3, 2, 2, 6);
	Point2D B(2, -8);

	double qform(const Point2D& x)
	{
		return .5 * (x * A * x) - x * B;
	}

	Point2D rotate(const Point3D& p)
	{
		const double rot_x = -pi/4; // rotation angle around axis X
		const double rot_z = -pi/4; // rotation angle around axis Z
		const double c_x = std::cos(rot_x);
		const double s_x = std::sin(rot_x);
		const double c_z = std::cos(rot_z);
		const double s_z = std::sin(rot_z);

		return Point2D
		(
			c_z * p.x - s_z * p.y,
			c_x * s_z * p.x + c_x * c_z * p.y - s_x * p.z
		);
	}

	void store_figure_quadratic_form()
	{
		double min_x = -2;
		double max_x = 4.5;
		double min_y = -4;
		double max_y = 2;
		double step = 0.5;

		Paint paint("figure-quadratic-form.tex");

		for(double x = min_x; x <= max_x; x += step)
		{
			for(double y = min_y; y <= max_y; y += step)
			{
				Point3D p0(x, y, scale * qform(Point2D(x, y)));
				Point3D p1(x-step, y, scale * qform(Point2D(x-step, y)));
				Point3D p2(x, y-step, scale * qform(Point2D(x, y-step)));

				Point2D p0r = rotate(p0);
				Point2D p1r = rotate(p1);
				Point2D p2r = rotate(p2);

				if(x != min_x)
					paint.line(p0r, p1r, Paint::BLUE);
				if(y != min_y)
					paint.line(p0r, p2r, Paint::BLUE);
			}
		}

		Point3D p(min_x, min_y, -0.3);
		Point3D px(p.x+7, p.y, p.z);
		Point3D py(px.x, px.y+7, px.z);
		Point3D psi(p.x, p.y, p.z+7);

		Point2D pr = rotate(p);
		Point2D pxr = rotate(px);
		Point2D pyr = rotate(py);
		Point2D psir = rotate(psi);

		paint.line(pr, pxr, Paint::ARROW);
		paint.line(pxr, pyr, Paint::ARROW);
		paint.line(pr, psir, Paint::ARROW);

		paint.text("$x$", pxr, Paint::BELOW);
		paint.text("$y$", pyr, Paint::BELOW | Paint::RIGHT);
		paint.text("$\\psi$", psir, Paint::LEFT);

		for(double i = 1.0; i < 7; i += 2)
		{
			Point3D px(p.x+i, p.y, p.z);
			Point3D py(p.x+7, p.y+i, p.z);
			Point3D pz(p.x, p.y, p.z+i);

			Point2D pxr = rotate(px);
			Point2D pyr = rotate(py);
			Point2D pzr = rotate(pz);

			paint.dot(pxr);
			paint.text(QString::number(px.x), pxr, Paint::BELOW | Paint::LEFT);
			paint.dot(pyr);
			paint.text(QString::number(py.y), pyr, Paint::BELOW | Paint::RIGHT);
			paint.dot(pzr);
			paint.text(QString::number(pz.z / scale, 'f', 0), pzr, Paint::LEFT);
		}

		Point2D x0(-0.5, 1.2);
		Point3D x0_3D(x0, scale * qform(x0));
		Point2D x0r = rotate(x0_3D);

		for(int i = 0; i < 100; i++)
		{
			Point2D r = B - A * x0;
			double alpha = (r * r) / (r * A * r);
			Point2D x1 = x0 + alpha * r;

			Point3D x1_3D(x1, scale * qform(x1));
			Point2D x1r = rotate(x1_3D);
			paint.line(x0r, x1r, Paint::RED | Paint::THICK);

			if(Point2D::norm2(x0, x1) < 0.0001)
			{
				break;
			}

			x0 = x1;
			x0r = x1r;
		}
	}

	void store_figure_levels()
	{
		Paint paint("figure-levels.tex");
		paint.line(Point2D(-4,0), Point2D(8,0), Paint::DARROW);
		paint.line(Point2D(0,-6), Point2D(0,2), Paint::DARROW);

		Point2D x0(-0.5, 1.2);
		for(int i = 0; i < 9; i++)
		{
			double step = Point2D::norm(x0, Point2D(2.0, -2.0)) / 7.0;
			double psi = qform(x0);
			Point2D xi = x0, stop;
			QList<Point2D> coord_list;
			for(int i = 0; i < 500; i++)
			{
				// find path of current circle
				Point2D r = B - A * xi;
				Point2D tan(r[1], -r[0]);
				xi = xi + step * tan / tan.norm();
				if(fabs(r[0]) < fabs(r[1]))
				{
					double a = .5 * A[1][1];
					double b = .5 * ( A[0][1] + A[1][0] ) * xi[0] - B[1];
					double c = .5 * A[0][0] * xi[0] * xi[0] - B[0] * xi[0] - psi;
					double delta = b * b - 4 * a * c;

					if(delta < 0.0) qWarning("delta < 0");

					delta = std::sqrt(delta);
					double x1a = (-b-delta) / (2*a);
					double x1b = (-b+delta) / (2*a);
					xi[1] = (fabs(xi[1] - x1a) < fabs(xi[1] - x1b)) ? x1a : x1b;
				}
				else
				{
					double a = .5 * A[0][0];
					double b = .5 * ( A[0][1] + A[1][0] ) * xi[1] - B[0];
					double c = .5 * A[1][1] * xi[1] * xi[1] - B[1] * xi[1] - psi;
					double delta = b * b - 4 * a * c;

					if(delta < 0.0) qWarning("delta < 0");

					delta = std::sqrt(delta);
					double x0a = (-b-delta) / (2*a);
					double x0b = (-b+delta) / (2*a);
					xi[0] = (fabs(xi[0] - x0a) < fabs(xi[0] - x0b)) ? x0a : x0b;
				}
				coord_list.append(xi);

				// break loop
				if(i == 0)
				{
					stop = xi;
				}
				else if(i > 1 && Point2D::norm(xi, stop) < step)
				{
					break;
				}
			}
			paint.spline(coord_list);

			// step up to next inner circle
			Point2D r = B - A * x0;
			double alpha = (r * r) / (r * A * r);
			Point2D x1 = x0 + alpha * r;

			paint.line(x0, x1, Paint::RED | Paint::THICK);
			x0 = x1;
		}
	}
}
