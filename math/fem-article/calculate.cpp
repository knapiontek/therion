double dP[dof_no] = { 0.0 }; // K * dP = F, delta P - solution vector

// solving K * dP = F by LU method
void calculate_equation()
{
	double L[dof_no][dof_no] = { { 0.0 } }; // lower matrix
	double U[dof_no][dof_no] = { { 0.0 } }; // upper matrix
	double Z[dof_no] = { 0.0 }; // auxiliary vector

	// init L := I
	for(int i = 0; i < dof_no; i++)
	{
		L[i][i] = 1.0;
	}

	// find L, U where L * U = K
	for(int i1 = 0; i1 < dof_no; i1++)
	{
		double acc = 0.0;
		for(int i2 = 0; i2 < dof_no; i2++)
			acc += L[i1][i2] * U[i2][i1];
		U[i1][i1] = K[i1][i1] - acc;

		for(int i2 = i1 + 1; i2 < dof_no; i2++)
		{
			acc = 0.0;
			for(int i3 = 0; i3 < i1; i3++)
				acc += L[i1][i3] * U[i3][i2];
			U[i1][i2] = K[i1][i2] - acc;

			acc = 0.0;
			for(int i3 = 0; i3 < i1; i3++)
				acc += L[i2][i3] * U[i3][i1];
			L[i2][i1] = (K[i2][i1] - acc) / U[i1][i1];
		}
	}

	// finally find result
	for(int i1 = 0; i1 < dof_no; i1++)
	{
		// find Z where L * Z = F
		double acc = 0.0;
		for(int i2 = 0; i2 < i1; i2++)
			acc += L[i1][i2] * Z[i2];
		Z[i1] = F[i1] - acc;
	}
	for(int i1 = dof_no - 1; i1 >= 0; i1--)
	{
		// find dP where U * dP = Z
		double acc = 0.0;
		for(int i2 = i1; i2 < dof_no; i2++)
			acc += U[i1][i2] * dP[i2];
		dP[i1] = (Z[i1] - acc) / U[i1][i1];
	}

	// copy to global output
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& point = point_list[i];
		const Point3D& fix = fix_list[i];
		Point3D& force = force_list[i];
		Point3D& output = output_list[i];

		int px = 3 * i + 0;
		int py = 3 * i + 1;
		int pz = 3 * i + 2;
		output = point;

		if(fix.x) force.x = dP[px];
		else output.x += dP[px];

		if(fix.y) force.y = dP[py];
		else output.y += dP[py];

		if(fix.z) force.z = dP[pz];
		else output.z += dP[pz];
	}
}
