
class Matrix2D
{
public:
	Matrix2D(double d00, double d01, double d10, double d11)
	{
		data[0][0] = d00;
		data[0][1] = d01;
		data[1][0] = d10;
		data[1][1] = d11;
	}
	double* operator[](int row)
	{
		return data[row];
	}
	const double* operator[](int row) const
	{
		return data[row];
	}
private:
	double data[2][2];
};

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
	double& operator[](int i)
	{
		return i ? y : x;
	}
	const double& operator[](int i) const
	{
		return i ? y : x;
	}
	double norm()
	{
		return std::sqrt(x * x + y * y);
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
	friend Point2D operator*(const Point2D& p, const Matrix2D& m)
	{
		return Point2D(p.x * m[0][0] + p.y * m[1][0], p.x * m[0][1] + p.y * m[1][1]);
	}
	friend Point2D operator*(const Matrix2D& m, const Point2D& p)
	{
		return Point2D(m[0][0] * p.x + m[0][1] * p.y, m[1][0] * p.x + m[1][1] * p.y);
	}
	static Point2D& nil()
	{
		const double infinity = std::numeric_limits<double>::infinity();
		static Point2D nil(infinity, infinity);
		return nil;
	}
	static bool equal(const Point2D& p1, const Point2D& p2)
	{
		return std::abs(p1.x - p2.x) < 1e-10 && std::abs(p1.y - p2.y) < 1e-10;
	}
	static double norm(const Point2D& p1, const Point2D& p2)
	{
		double dx = p2.x - p1.x;
		double dy = p2.y - p1.y;
		return std::sqrt(dx * dx + dy * dy);
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
					result = b1 + scale * u;
				}
			}
		}

		return result;
	}
};

struct Point3D : public Point2D
{
	double z;

	Point3D() : Point2D()
	{
		z = 0.0;
	}
	Point3D(double x, double y, double z) : Point2D(x, y)
	{
		this->z = z;
	}
	Point3D(const Point2D& p, double z = 0.0) : Point2D(p)
	{
		this->z = z;
	}
	Point3D(const Point3D& p) : Point2D(p)
	{
		z = p.z;
	}
	friend Point3D operator+(const Point3D& p1, const Point3D& p2)
	{
		return Point3D(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
	}
	friend Point3D operator-(const Point3D& p1, const Point3D& p2)
	{
		return Point3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
	}
	friend Point3D operator*(double d, const Point3D& p)
	{
		return Point3D(d * p.x, d * p.y, d * p.z);
	}
};
