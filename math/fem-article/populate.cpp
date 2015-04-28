const int dof_no = 3 * point_no; // degrees of freedom
double K[dof_no][dof_no] = { { 0.0 } }; // stiffness matrix
double F[dof_no] = { 0.0 }; // force vector

void populate_equation()
{
	// init F, K
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& fix = fix_list[i];
		Point3D& force = force_list[i];
		int px = 3 * i + 0;
		int py = 3 * i + 1;
		int pz = 3 * i + 2;

		// reactions in K (in place of fixed displacement)
		if(fix.x) K[px][px] = -1;
		else F[px] = force.x;

		if(fix.y) K[py][py] = -1;
		else F[py] = force.y;

		if(fix.z) K[pz][pz] = -1;
		else F[pz] = force.z;
	}

	// compose K - stiffness matrix
	for(int i = 0; i < element_no; i++)
	{
		const double EA = 1000; // Young * Area
		const Element& element = element_list[i];
		const Point3D& point1 = point_list[element.p1];
		const Point3D& point2 = point_list[element.p2];
		const Point3D& fix1 = fix_list[element.p1];
		const Point3D& fix2 = fix_list[element.p2];
		int p1x = 3 * element.p1 + 0;
		int p1y = 3 * element.p1 + 1;
		int p1z = 3 * element.p1 + 2;
		int p2x = 3 * element.p2 + 0;
		int p2y = 3 * element.p2 + 1;
		int p2z = 3 * element.p2 + 2;
		double dx = point2.x - point1.x;
		double dy = point2.y - point1.y;
		double dz = point2.z - point1.z;
		double l = ::sqrt(dx * dx + dy * dy + dz * dz);
		double cx = dx / l;
		double cy = dy / l;
		double cz = dz / l;
		double cxxEAl = cx * cx * EA / l;
		double cyyEAl = cy * cy * EA / l;
		double czzEAl = cz * cz * EA / l;
		double cxyEAl = cx * cy * EA / l;
		double cxzEAl = cx * cz * EA / l;
		double cyzEAl = cy * cz * EA / l;

		if(!fix1.x)
		{
			K[p1x][p1x] += cxxEAl;
			K[p1y][p1x] += cxyEAl;
			K[p1z][p1x] += cxzEAl;
			K[p2x][p1x] -= cxxEAl;
			K[p2y][p1x] -= cxyEAl;
			K[p2z][p1x] -= cxzEAl;
		}
		if(!fix1.y)
		{
			K[p1x][p1y] += cxyEAl;
			K[p1y][p1y] += cyyEAl;
			K[p1z][p1y] += cyzEAl;
			K[p2x][p1y] -= cxyEAl;
			K[p2y][p1y] -= cyyEAl;
			K[p2z][p1y] -= cyzEAl;
		}
		if(!fix1.z)
		{
			K[p1x][p1z] += cxzEAl;
			K[p1y][p1z] += cyzEAl;
			K[p1z][p1z] += czzEAl;
			K[p2x][p1z] -= cxzEAl;
			K[p2y][p1z] -= cyzEAl;
			K[p2z][p1z] -= czzEAl;
		}
		if(!fix2.x)
		{
			K[p1x][p2x] -= cxxEAl;
			K[p1y][p2x] -= cxyEAl;
			K[p1z][p2x] -= cxzEAl;
			K[p2x][p2x] += cxxEAl;
			K[p2y][p2x] += cxyEAl;
			K[p2z][p2x] += cxzEAl;
		}
		if(!fix2.y)
		{
			K[p1x][p2y] -= cxyEAl;
			K[p1y][p2y] -= cyyEAl;
			K[p1z][p2y] -= cyzEAl;
			K[p2x][p2y] += cxyEAl;
			K[p2y][p2y] += cyyEAl;
			K[p2z][p2y] += cyzEAl;
		}
		if(!fix2.z)
		{
			K[p1x][p2z] -= cxzEAl;
			K[p1y][p2z] -= cyzEAl;
			K[p1z][p2z] -= czzEAl;
			K[p2x][p2z] += cxzEAl;
			K[p2y][p2z] += cyzEAl;
			K[p2z][p2z] += czzEAl;
		}
	}
}
