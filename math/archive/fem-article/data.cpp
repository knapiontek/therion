struct Point3D
{
	double x, y, z;
};

// indices of first and second end of an element
struct Element
{
	int p1, p2;
};

const int point_no = 4;
const int element_no = 6;

// geometry nodes
const Point3D point_list[point_no] = {
	{-2.0,-2.0,-2.0 }, // 0
	{-2.0, 2.0,-2.0 }, // 1
	{ 2.5,-2.0,-2.0 }, // 2
	{-2.0,-2.0, 4.0 } // 3
};

// geometry, refers to point_list or/and output_list
const Element element_list[element_no] = {
	{ 0, 1 }, // 0
	{ 1, 2 }, // 1
	{ 2, 0 }, // 2
	{ 3, 0 }, // 3
	{ 3, 1 }, // 4
	{ 3, 2 } // 5
};

// fixed point displacement list, boolean values
Point3D fix_list[point_no] = {
	{ 1, 0, 1 }, // 0
	{ 1, 0, 1 }, // 1
	{ 0, 0, 0 }, // 2
	{ 1, 1, 1 } // 3
};

// forces attached to coresponding points
Point3D force_list[point_no] = {
	{ 0.0, 0.0, 0.0 }, // 0
	{ 0.0, 0.0, 0.0 }, // 1
	{ 0.0,30.0,30.0 }, // 2
	{ 0.0, 0.0, 0.0 } // 3
};

// displaced points
Point3D output_list[point_no] = {
	{ 0.0, 0.0, 0.0 }, // 0
	{ 0.0, 0.0, 0.0 }, // 1
	{ 0.0, 0.0, 0.0 }, // 2
	{ 0.0, 0.0, 0.0 } // 3
};
