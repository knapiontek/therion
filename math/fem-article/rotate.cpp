const double rot_x = .1; // rotation angle around axis X
const double rot_y = .1; // rotation angle around axis Z
const double c_x = ::cos(rot_x);
const double s_x = ::sin(rot_x);
const double c_y = ::cos(rot_y);
const double s_y = ::sin(rot_y);

// transform/rotate point from 3D => 2D
Point2D rotate(const Point3D& p)
{
	return Point2D
	(
		c_y * p.x - s_y * p.z,
		- s_x * s_y * p.x + c_x * p.y - s_x * c_y * p.z
	);
}
