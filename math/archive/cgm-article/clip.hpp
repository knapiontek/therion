
namespace clip
{
	struct Element
	{
		Point3D pt1, pt2;
		int p1, p2;
	};

	Point3D no_rotate(const Point3D& p)
	{
		return p;
	}

	void find_z(Point3D& cross, const Point3D& pt1, const Point3D& pt2)
	{
		Point2D a = cross - pt1;
		Point3D b = pt2 - pt1;
		cross.z = std::abs(b.x) > std::abs(b.y)
			? pt1.z + a.x * b.z / b.x
			: pt1.z + a.y * b.z / b.y;
	}

	template<typename Rotate>
	QVector<Element> make(const Point3D point_list[], const data::Element element_list[], int element_size, Rotate rotate)
	{
		const double inf = std::numeric_limits<double>::infinity();

		// init elements and join triangles
		QVector<Element> elements;
		struct Triangle { int p1, p2, p3; };
		QVector<Triangle> triangles;
		{
			QMultiMap<int, int> element_map;
			for(int i = 0; i < element_size; i++)
			{
				auto& e = element_list[i];
				if(e.p1 < e.p2)
					element_map.insert(e.p1, e.p2);
				else
					element_map.insert(e.p2, e.p1);
			}
			for(auto e = element_map.begin(); e != element_map.end(); e++)
			{
				int p1 = e.key();
				int p2 = e.value();

				auto pt1 = rotate(point_list[p1]);
				auto pt2 = rotate(point_list[p2]);
				elements.append(Element{ pt1, pt2, p1, p2 });

				for(int p3 : element_map.values(p2))
				{
					if(element_map.find(p1, p3) != element_map.end())
					{
						triangles.append(Triangle{ p1, p2, p3 });
					}
				}
			}
		}

		// clip elements by triangles
		for(int i = 0; i < elements.size(); i++)
		{
			for(auto& t : triangles)
			{
				auto& e = elements[i];

				Point2D v = e.pt2 - e.pt1;
				double n = v.norm();
				Point2D u = v / n;

				auto t1 = rotate(point_list[t.p1]);
				auto t2 = rotate(point_list[t.p2]);
				auto t3 = rotate(point_list[t.p3]);

				Point2D arm1 = t1 - e.pt1;
				Point2D arm2 = t2 - e.pt1;
				Point2D arm3 = t3 - e.pt1;

				double sin1 = arm1 ^ u;
				double sin2 = arm2 ^ u;
				double sin3 = arm3 ^ u;

				double cos1 = arm1 * u;
				double cos2 = arm2 * u;
				double cos3 = arm3 * u;

				struct { double scale, sina, sinb, cosa, cosb; int pa, pb; Point3D pta, ptb, cross, tcross; } list[]
				{
					{ inf, sin1, sin2, cos1, cos2, t.p1, t.p2, t1, t2, Point3D::nil(), Point3D::nil() },
					{ inf, sin1, sin3, cos1, cos3, t.p1, t.p3, t1, t3, Point3D::nil(), Point3D::nil() },
					{ inf, sin2, sin3, cos2, cos3, t.p2, t.p3, t2, t3, Point3D::nil(), Point3D::nil() }
				};

				for(auto& l : list)
				{
					if(e.p1 == l.pa && e.p2 == l.pb)
					{
						goto continue_triangles;
					}
					else if(e.p1 == l.pa || e.p1 == l.pb)
					{
						l.scale = 0.0;
						l.cross = l.tcross = e.pt1;
					}
					else if(e.p2 == l.pa || e.p2 == l.pb)
					{
						l.scale = n;
						l.cross = l.tcross = e.pt2;
					}
					else if(l.sina * l.sinb <= 0)
					{
						double sin = l.sina - l.sinb;
						if(sin != 0)
						{
							double ratio = (l.cosb - l.cosa) * l.sina / sin;
							l.scale = l.cosa + ratio;
							l.cross = l.tcross = e.pt1 + Point3D(l.scale * u);

							find_z(l.cross, e.pt1, e.pt2);
							find_z(l.tcross, l.pta, l.ptb);

							if(l.scale > 0 && l.scale < n && l.cross.z > l.tcross.z)
							{
								goto continue_triangles;
							}
						}
					}
				}

				if(list[1].scale > list[2].scale)
					qSwap(list[1], list[2]);
				if(list[0].scale > list[1].scale)
					qSwap(list[0], list[1]);
				if(list[1].scale > list[2].scale)
					qSwap(list[1], list[2]);
				if(list[2].scale < inf)
					list[1] = list[2];

				if(list[0].scale > 0 && list[0].scale < n)
				{
					Point3D ept2 = e.pt2;
					e.pt2 = list[0].cross;
					if(list[1].scale > 0 && list[1].scale < n)
					{
						Element el { list[1].cross, ept2, e.p1, e.p2 };
						elements.append(el);
					}
				}
				else if(list[1].scale > 0 && list[1].scale < n)
				{
					e.pt1 = list[1].cross;
				}
				else if(list[0].scale <= 0 && list[1].scale >= n)
				{
					if(e.p1 != list[0].pa)
					{
						Point3D tcross1 = e.pt1;
						find_z(tcross1, list[0].tcross, list[1].tcross);
						if(e.pt1.z < tcross1.z)
						{
							e.pt1 = e.pt2 = Point3D::nil();
							break;
						}
					}
					if(e.p2 != list[1].pb)
					{
						Point3D tcross2 = e.pt2;
						find_z(tcross2, list[0].tcross, list[1].tcross);
						if(e.pt2.z < tcross2.z)
						{
							e.pt1 = e.pt2 = Point3D::nil();
							break;
						}
					}
				}

				continue_triangles:
				continue;
			}
		}

		return elements;
	}
}
