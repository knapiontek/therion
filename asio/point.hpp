
struct Point2D
{
	double x, y;

	Point2D()
	{
		x = 0.0;
		y = 0.0;
	}
	Point2D(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	Point2D(const Point2D& p)
	{
		x = p.x;
		y = p.y;
	}
	friend bool operator==(const Point2D& p1, const Point2D& p2)
	{
		return p1.x == p2.x && p1.y == p2.y;
	}
	friend bool operator!=(const Point2D& p1, const Point2D& p2)
	{
		return p1.x != p2.x || p1.y != p2.y;
	}
	friend Point2D operator+(const Point2D& p1, const Point2D& p2)
	{
		return Point2D(p1.x + p2.x, p1.y + p2.y);
	}
	friend Point2D operator-(const Point2D& p1, const Point2D& p2)
	{
		return Point2D(p1.x - p2.x, p1.y - p2.y);
	}
	friend Point2D operator*(double d, const Point2D& p)
	{
		return Point2D(d * p.x, d * p.y);
	}
	friend Point2D operator/(const Point2D& p, double d)
	{
		return Point2D(p.x / d, p.y / d);
	}
	friend double operator*(const Point2D& p1, const Point2D& p2)
	{
		return p1.x * p2.x + p1.y * p2.y;
	}
	friend double operator^(const Point2D& p1, const Point2D& p2)
	{
		return p1.x * p2.y - p1.y * p2.x;
	}
	static Point2D& nil()
	{
		const double infinity = std::numeric_limits<double>::infinity();
		static Point2D nil(infinity, infinity);
		return nil;
	}
	double norm()
	{
		return std::sqrt(x * x + y * y);
	}
	static double norm(const Point2D& p1, const Point2D& p2)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		return std::sqrt(dx * dx + dy * dy);
	}
	double norm2()
	{
		return x * x + y * y;
	}
	static double norm2(const Point2D& p1, const Point2D& p2)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		return dx * dx + dy * dy;
	}
	static Point2D intersec(const Point2D& b1, const Point2D& e1, const Point2D& b2, const Point2D& e2)
	{
		Point2D result = nil();

		Point2D v = e1 - b1;
		double n = v.norm();
		Point2D u = v / n;

		Point2D seg1 = b2 - b1;
		Point2D seg2 = e2 - b1;

		double cos1 = seg1 * u;
		double cos2 = seg2 * u;

		double sin1 = seg1 ^ u;
		double sin2 = seg2 ^ u;

		if(sin1 * sin2 <= 0)
		{
			double sin = sin1 - sin2;
			if(sin)
			{
				double ratio = (cos2 - cos1) * sin1 / sin; // Congruent Triangles
				double scale = cos1 + ratio;
				if(0 < scale && scale < n)
				{
					result = b1 + scale * u;
				}
			}
		}

		return result;
	}
};
