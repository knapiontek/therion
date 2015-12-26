
namespace solve_tetrahedron
{
	const int point_size = sizeof(tetrahedron::point_list) / sizeof(Point3D);
	const int element_size = sizeof(tetrahedron::element_list) / sizeof(data::Element);
	const int fix_size = sizeof(tetrahedron::fix_list) / sizeof(data::Fix);
	const int force_size = sizeof(tetrahedron::force_list) / sizeof(data::Force);

	Point3D displace_list[point_size] = tetrahedron::point_list;
	data::Force reaction_list[fix_size];

	void store_equation_K(const sp::Matrix kmx, const sp::Vector& dp, const sp::Vector& F)
	{
		QVector<double> K(sp::size * sp::size);
		sp::copy(K, kmx);

		QFile file("equation_K.tex");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream stream(&file);
		stream.setRealNumberPrecision(4);

		stream << "\\begin{equation}\n\\begin{bmatrix}\n";
		for(int i1 = 0; i1 < sp::size; i1++)
		{
			if(i1) stream << " \\\\\n";
			for(int i2 = 0; i2 < sp::size; i2++)
			{
				if(i2) stream << " & ";
				if(0.0 == K[i1 * sp::size + i2]) stream << ".";
				else stream << K[i1 * sp::size + i2];
			}
		}
		stream << "\n\\end{bmatrix}\n\\begin{bmatrix}\n";
		for(int i = 0; i < sp::size; i++)
		{
			if(i) stream << " \\\\\n";
			if(0.0 == dp[i]) stream << ".";
			else stream << dp[i];
		}
		stream << "\n\\end{bmatrix}=\n\\begin{bmatrix}\n";
		for(int i = 0; i < sp::size; i++)
		{
			if(i) stream << " \\\\\n";
			if(0.0 == F[i]) stream << ".";
			else stream << F[i];
		}
		stream << "\n\\end{bmatrix}\n\\end{equation}";
	}

	void store_equation_T(const sp::Matrix tmx, const sp::Vector& dp, const QVector<double>& reaction)
	{
		QVector<double> T(reaction.size() * sp::size);
		sp::copy(T, tmx);

		QFile file("equation_T.tex");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream stream(&file);
		stream.setRealNumberPrecision(4);

		stream << "\\begin{equation}\n\\begin{bmatrix}\n";
		for(int i1 = 0; i1 < reaction.size(); i1++)
		{
			if(i1) stream << " \\\\\n";
			for(int i2 = 0; i2 < sp::size; i2++)
			{
				if(i2) stream << " & ";
				if(0.0 == T[i1 * sp::size + i2]) stream << ".";
				else stream << T[i1 * sp::size + i2];
			}
		}
		stream << "\n\\end{bmatrix}\n\\begin{bmatrix}\n";
		for(int i = 0; i < sp::size; i++)
		{
			if(i) stream << " \\\\\n";
			if(0.0 == dp[i]) stream << ".";
			else stream << dp[i];
		}
		stream << "\n\\end{bmatrix}=\n\\begin{bmatrix}\n";
		for(int i = 0; i < reaction.size(); i++)
		{
			if(i) stream << " \\\\\n";
			if(0.0 == reaction[i]) stream << ".";
			else stream << reaction[i];
		}
		stream << "\n\\end{bmatrix}\n\\end{equation}";
	}

	Point2D rotate(const Point3D& p)
	{
		const double rot_x = .1; // rotation angle around axis X
		const double rot_y = .1; // rotation angle around axis Z
		const double c_x = std::cos(rot_x);
		const double s_x = std::sin(rot_x);
		const double c_y = std::cos(rot_y);
		const double s_y = std::sin(rot_y);

		return Point2D
		(
			c_y * p.x - s_y * p.z,
			- s_x * s_y * p.x + c_x * p.y - s_x * c_y * p.z
		);
	}

	Point2D west(const Point3D& p)
	{
		return Point2D
		(
			- 4 + p.x,
			2 + p.y
		);
	}

	Point2D east(const Point2D& p)
	{
		return Point2D
		(
			3 + p.x,
			2 + p.y
		);
	}

	void store_figure_tetrahedron()
	{
		Paint paint("figure-tetrahedron.tex");

		// draw unit vectors
		const Point3D p0 = { -4, 0, 0 };
		Point3D px = { 1 + p0.x, p0.y, p0.z };
		Point3D py = { p0.x, 1 + p0.y, p0.z };
		Point3D pz = { p0.x, p0.y, 3 + p0.z };

		Point2D p0r = rotate(p0);
		Point2D pxr = rotate(px);
		Point2D pyr = rotate(py);
		Point2D pzr = rotate(pz);

		Point2D p0m = east(p0r);
		Point2D pxm = east(pxr);
		Point2D pym = east(pyr);
		Point2D pzm = east(pzr);

		paint.line(p0m, pxm, Paint::ARROW);
		paint.text("$x$", pxm, Paint::BELOW);

		paint.line(p0m, pym, Paint::ARROW);
		paint.text("$y$", pym, Paint::LEFT);

		paint.line(p0m, pzm, Paint::ARROW);
		paint.text("$z$", pzm, Paint::LEFT);

		// draw point_list
		for(int i = 0; i < element_size; i++)
		{
			auto& element = tetrahedron::element_list[i];
			auto& p1 = tetrahedron::point_list[element.p1];
			auto& p2 = tetrahedron::point_list[element.p2];

			Point2D p1r = rotate(p1);
			Point2D p2r = rotate(p2);

			Point2D p1m = east(p1r);
			Point2D p2m = east(p2r);

			paint.line(p1m, p2m);
		}

		// draw displace_list
		for(int i = 0; i < element_size; i++)
		{
			auto& element = tetrahedron::element_list[i];

			auto& p1 = displace_list[element.p1];
			auto& p2 = displace_list[element.p2];

			Point2D p1r = rotate(p1);
			Point2D p2r = rotate(p2);

			Point2D p1m = east(p1r);
			Point2D p2m = east(p2r);

			paint.line(p1m, p2m, Paint::DASH);
		}

		// draw forces
		for(int i = 0; i < force_size; i++)
		{
			auto& force = tetrahedron::force_list[i];
			auto& point = displace_list[force.p];

			Point2D p1r = rotate(point);
			Point2D p2r = rotate(point + 0.05 * force.val);

			Point2D p1m = east(p1r);
			Point2D p2m = east(p2r);

			paint.line(p1m, p2m, Paint::RED | Paint::ARROW | Paint::THICK);
		}

		// draw reactions
		for(int i = 0; i < fix_size; i++)
		{
			auto& reaction = reaction_list[i];
			auto& point = displace_list[reaction.p];

			Point2D p1r = rotate(point);
			Point2D p2r = rotate(point + 0.05 * reaction.val);

			Point2D p1m = east(p1r);
			Point2D p2m = east(p2r);

			paint.line(p1m, p2m, Paint::RED | Paint::ARROW | Paint::THICK);
		}
	}

	void run()
	{
		// init
		sp::size = convert::init(tetrahedron::fix_list, point_size);

		// populate kmx, tmx
		convert::matrix(tetrahedron::point_list, tetrahedron::element_list, element_size);

		sp::SymetryMatrix K = convert::kmx;

		// populate F
		auto F = convert::force(tetrahedron::force_list, force_size);

		// solve K * dp = F
		sp::Vector dp;
		for(int i = 0; i < 1000; i++)
		{
			auto r = K * dp;
			r -= F;
			r *= -1.0; // r := F - K * dp
			double a = (r * r) / (r * K * r);
			r *= a;
			dp += r; // dp := dp + a * r
		}

		Q_ASSERT(sp::Vector::norm2(K * dp, F) < 0.001);

		// solve reactions
		QVector<double> reaction(3 * point_size - sp::size);
		sp::mul(reaction, convert::tmx, dp);

		// convert back to 3D domain
		convert::displace(displace_list, dp);
		convert::reaction(reaction_list, tetrahedron::fix_list, fix_size, reaction);

		// store equations and final figure
		store_equation_K(convert::kmx, dp, F);
		store_equation_T(convert::tmx, dp, reaction);
		store_figure_tetrahedron();
	}
}
