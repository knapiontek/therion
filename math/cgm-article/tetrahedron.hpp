
namespace tetrahedron
{
	// geometry nodes
	const Point3D point_list[]
	{
		{ -2.0, -2.0, -2.0 }, // 0
		{ -2.0,  2.0, -2.0 }, // 1
		{  2.5, -2.0, -2.0 }, // 2
		{ -2.0, -2.0,  4.0 } // 3
	};

	// geometry, refers to point_list (first and second end of an element)
	const data::Element element_list[]
	{
		{ 0, 1 }, // 0
		{ 1, 2 }, // 1
		{ 2, 0 }, // 2
		{ 3, 0 }, // 3
		{ 3, 1 }, // 4
		{ 3, 2 } // 5
	};

	// fixed point displacement list (point id and fixed dimentions)
	const data::Fix fix_list[]
	{
		{ 0, data::X | data::Z },
		{ 1, data::X | data::Z },
		{ 3, data::X | data::Y | data::Z }
	};

	// forces attached to coresponding points
	const data::Force force_list[]
	{
		{ 2, { 0.0, 30.0, 30.0 } }
	};
}
