#include "engine.h"
#include "input.h"

#include <math.h>

// LU decompotion data

double K[DOF_NO][DOF_NO] = { { 0.0 } };
double L[DOF_NO][DOF_NO] = { { 0.0 } };
double U[DOF_NO][DOF_NO] = { { 0.0 } };
double X[DOF_NO] = { 0.0 };
double Z[DOF_NO] = { 0.0 };
double F[DOF_NO] = { 0.0 };

void engine_init()
{
	// init
	for(auto i = 0; i < NODE_NO; i++)
	{
		auto& data = data_list[i];
		auto nx = 3 * i + 0;
		auto ny = 3 * i + 1;
		auto nz = 3 * i + 2;

		// init L := I, F
		L[nx][nx] = 1.0;
		L[ny][ny] = 1.0;
		L[nz][nz] = 1.0;
		F[nx] = data.force.x;
		F[ny] = data.force.y;
		F[nz] = data.force.z;

		// reactions in K (in place of fixed displacement)
		if(data.fix)
		{
			K[nx][nx] = -1;
			K[ny][ny] = -1;
			K[nz][nz] = -1;
		}
	}

	// compose K - stiffness matrix
	for(auto i = 0; i < ELEMENT_NO; i++)
	{
		auto& element = element_list[i];
		auto n1x = 3 * element.n1 + 0;
		auto n1y = 3 * element.n1 + 1;
		auto n1z = 3 * element.n1 + 2;
		auto n2x = 3 * element.n2 + 0;
		auto n2y = 3 * element.n2 + 1;
		auto n2z = 3 * element.n2 + 2;
		auto& data1 = data_list[element.n1];
		auto& data2 = data_list[element.n2];
		auto x = data2.node.x - data1.node.x;
		auto y = data2.node.y - data1.node.y;
		auto z = data2.node.z - data1.node.z;
		auto l = ::sqrt(x * x + y * y + z * z);
		auto cx = x / l;
		auto cy = y / l;
		auto cz = z / l;
		auto cxxl = cx * cx / l;
		auto cyyl = cy * cy / l;
		auto czzl = cz * cz / l;
		auto cxyl = cx * cy / l;
		auto cxzl = cx * cz / l;
		auto cyzl = cy * cz / l;

		if(!data1.fix)
		{
			K[n1x][n1x] += cxxl;
			K[n1y][n1x] += cxyl;
			K[n1z][n1x] += cxzl;
			K[n2x][n1x] -= cxxl;
			K[n2y][n1x] -= cxyl;
			K[n2z][n1x] -= cxzl;

			K[n1x][n1y] += cxyl;
			K[n1y][n1y] += cyyl;
			K[n1z][n1y] += cyzl;
			K[n2x][n1y] -= cxyl;
			K[n2y][n1y] -= cyyl;
			K[n2z][n1y] -= cyzl;

			K[n1x][n1z] += cxzl;
			K[n1y][n1z] += cyzl;
			K[n1z][n1z] += czzl;
			K[n2x][n1z] -= cxzl;
			K[n2y][n1z] -= cyzl;
			K[n2z][n1z] -= czzl;
		}
		if(!data2.fix)
		{
			K[n1x][n2x] -= cxxl;
			K[n1y][n2x] -= cxyl;
			K[n1z][n2x] -= cxzl;
			K[n2x][n2x] += cxxl;
			K[n2y][n2x] += cxyl;
			K[n2z][n2x] += cxzl;

			K[n1x][n2y] -= cxyl;
			K[n1y][n2y] -= cyyl;
			K[n1z][n2y] -= cyzl;
			K[n2x][n2y] += cxyl;
			K[n2y][n2y] += cyyl;
			K[n2z][n2y] += cyzl;

			K[n1x][n2z] -= cxzl;
			K[n1y][n2z] -= cyzl;
			K[n1z][n2z] -= czzl;
			K[n2x][n2z] += cxzl;
			K[n2y][n2z] += cyzl;
			K[n2z][n2z] += czzl;
		}
	}

	// find L, U where L x U = K
	for(auto i1 = 0; i1 < DOF_NO; i1++)
	{
		auto acc = 0.0;
		for(auto i2 = 0; i2 < DOF_NO; i2++)
			acc += L[i1][i2] * U[i2][i1];
		U[i1][i1] = K[i1][i1] - acc;

		for(auto i2 = i1 + 1; i2 < DOF_NO; i2++)
		{
			acc = 0.0;
			for(auto i3 = 0; i3 < i1; i3++)
				acc += L[i1][i3] * U[i3][i2];
			U[i1][i2] = K[i1][i2] - acc;

			acc = 0.0;
			for(auto i3 = 0; i3 < i1; i3++)
				acc += L[i2][i3] * U[i3][i1];
			L[i2][i1] = (K[i2][i1] - acc) / U[i1][i1];
		}
	}

	// finally find shift
	for(auto i1 = 0; i1 < DOF_NO; i1++)
	{
		// find Z where L x Z = Y, apply F
		auto acc = 0.0;
		for(auto i2 = 0; i2 < i1; i2++)
			acc += L[i1][i2] * Z[i2];
		Z[i1] = F[i1] - acc;
	}
	for(auto i1 = DOF_NO - 1; i1 >= 0; i1--)
	{
		// find X where U x X = Z
		auto acc = 0.0;
		for(auto i2 = i1; i2 < DOF_NO; i2++)
			acc += U[i1][i2] * X[i2];
		X[i1] = (Z[i1] - acc) / U[i1][i1];
	}

	// copy to original inout
	auto scale = 1000.0; // 1/EA
	for(auto i = 0; i < NODE_NO; i++)
	{
		auto& data = data_list[i];
		data.shift = data.node;
		auto nx = 3 * i + 0;
		auto ny = 3 * i + 1;
		auto nz = 3 * i + 2;
		if(data.fix)
		{
			data.force.x = X[nx];
			data.force.y = X[ny];
			data.force.z = X[nz];
		}
		else
		{
			data.shift.x += X[nx] / scale;
			data.shift.y += X[ny] / scale;
			data.shift.z += X[nz] / scale;
		}
	}
}
