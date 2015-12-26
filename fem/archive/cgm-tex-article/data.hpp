
namespace data
{
	struct Element
	{
		int p1, p2;
	};

	enum
	{
		X = 1,
		Y = 2,
		Z = 4
	};

	struct Fix
	{
		int p;
		int flag;
	};

	struct Force
	{
		int p;
		Point3D val;
	};
}
