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
		auto n = 2 * i;
		auto& data = data_list[i];

		// init L := I, F
		L[n][n] = 1.0;
		L[n + 1][n + 1] = 1.0;
		F[n] = data.force.x;
		F[n + 1] = data.force.y;

		// reactions in K (in place of fixed displacement)
		if(0.0 == data.fix.x)
		{
			K[n][n] = -1;
		}
		if(0.0 == data.fix.y)
		{
			K[n + 1][n + 1] = -1;
		}
	}

	// compose K - stiffness matrix
	for(auto i = 0; i < ELEMENT_NO; i++)
	{
		auto& element = element_list[i];
		auto& data1 = data_list[element.n1];
		auto& data2 = data_list[element.n2];
		auto x = data2.node.x - data1.node.x;
		auto y = data2.node.y - data1.node.y;
		auto l = ::sqrt(x * x + y * y);
		auto n1 = 2 * element.n1; // node index => position in K
		auto n2 = 2 * element.n2;
		auto c = x / l;
		auto s = y / l;
		auto ccl = c * c / l;
		auto csl = c * s / l;
		auto ssl = s * s / l;

		if(0.0 != data1.fix.x)
		{
			K[n1][n1] += ccl;
			K[n1 + 1][n1] += csl;
			K[n2][n1] += -ccl;
			K[n2 + 1][n1] += -csl;
		}
		if(0.0 != data1.fix.y)
		{
			K[n1][n1 + 1] += csl;
			K[n1 + 1][n1 + 1] += ssl;
			K[n2][n1 + 1] += -csl;
			K[n2 + 1][n1 + 1] += -ssl;
		}
		if(0.0 != data2.fix.x)
		{
			K[n1][n2] += -ccl;
			K[n1 + 1][n2] += -csl;
			K[n2][n2] += ccl;
			K[n2 + 1][n2] += csl;
		}
		if(0.0 != data2.fix.y)
		{
			K[n1][n2 + 1] += -csl;
			K[n1 + 1][n2 + 1] += -ssl;
			K[n2][n2 + 1] += csl;
			K[n2 + 1][n2 + 1] += ssl;
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

	// translate back
	auto scale = 1000.0; // 1/EA
	for(auto i = 0; i < NODE_NO; i++)
	{
		auto& data = data_list[i];
		data.shift = data.node;
		if(0.0 == data.fix.x)
		{
			data.force.x += X[2 * i];
		}
		else
		{
			data.shift.x += X[2 * i] / scale;
		}
		if(0.0 == data.fix.y)
		{
			data.force.y += X[2 * i + 1];
		}
		else
		{
			data.shift.y += X[2 * i + 1] / scale;
		}
	}
}
