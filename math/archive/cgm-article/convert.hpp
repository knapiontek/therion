
namespace convert
{
	sp::Matrix kmx;
	sp::Matrix tmx;

	QVector<int> data_in;
	QVector<int> data_out;

	void set(int row, int col, double val)
	{
		int math_row = data_in[row];
		int math_col = data_in[col];

		if(math_col < sp::size && val != 0.0)
		{
			if(math_row < sp::size)
			{
				kmx.set(math_row, math_col, val);
			}
			else
			{
				tmx.set(math_row - sp::size, math_col, val);
			}
		}
	}

	int init(const data::Fix fix_list[], int point_size)
	{
		data_in.resize(3 * point_size);
		data_out.resize(3 * point_size);

		int in = 0, out = 0, top = 3 * point_size;
		auto fix = fix_list;
		while(in < 3 * point_size)
		{
			if(in == 3 * fix->p)
			{
				for(auto flag : { data::X, data::Y, data::Z })
				{
					if(fix->flag & flag)
					{
						top--;
						data_in[in] = top;
						data_out[top] = in;
					}
					else
					{
						data_in[in] = out;
						data_out[out] = in;
						out++;
					}
					in++;
				}
				fix++;
			}
			else
			{
				data_in[in] = out;
				data_out[out] = in;
				in++;
				out++;
			}
		}
		return out;
	}

	void matrix(const Point3D point_list[], const data::Element element_list[], int element_size)
	{
		for(int i = 0; i < element_size; i++)
		{
			const double EA = 1000.0; // Young * Area

			auto& element = element_list[i];

			auto& point1 = point_list[element.p1];
			auto& point2 = point_list[element.p2];

			int p1x = 3 * element.p1 + 0;
			int p1y = 3 * element.p1 + 1;
			int p1z = 3 * element.p1 + 2;
			int p2x = 3 * element.p2 + 0;
			int p2y = 3 * element.p2 + 1;
			int p2z = 3 * element.p2 + 2;

			double dx = point2.x - point1.x;
			double dy = point2.y - point1.y;
			double dz = point2.z - point1.z;

			double l = std::sqrt(dx * dx + dy * dy + dz * dz);

			double cx = dx / l;
			double cy = dy / l;
			double cz = dz / l;

			double cxxEAl = cx * cx * EA / l;
			double cyyEAl = cy * cy * EA / l;
			double czzEAl = cz * cz * EA / l;
			double cxyEAl = cx * cy * EA / l;
			double cxzEAl = cx * cz * EA / l;
			double cyzEAl = cy * cz * EA / l;

			set(p1x, p1x, cxxEAl);
			set(p1y, p1x, cxyEAl);
			set(p1z, p1x, cxzEAl);
			set(p2x, p1x, -cxxEAl);
			set(p2y, p1x, -cxyEAl);
			set(p2z, p1x, -cxzEAl);

			set(p1x, p1y, cxyEAl);
			set(p1y, p1y, cyyEAl);
			set(p1z, p1y, cyzEAl);
			set(p2x, p1y, -cxyEAl);
			set(p2y, p1y, -cyyEAl);
			set(p2z, p1y, -cyzEAl);

			set(p1x, p1z, cxzEAl);
			set(p1y, p1z, cyzEAl);
			set(p1z, p1z, czzEAl);
			set(p2x, p1z, -cxzEAl);
			set(p2y, p1z, -cyzEAl);
			set(p2z, p1z, -czzEAl);

			set(p1x, p2x, -cxxEAl);
			set(p1y, p2x, -cxyEAl);
			set(p1z, p2x, -cxzEAl);
			set(p2x, p2x, cxxEAl);
			set(p2y, p2x, cxyEAl);
			set(p2z, p2x, cxzEAl);

			set(p1x, p2y, -cxyEAl);
			set(p1y, p2y, -cyyEAl);
			set(p1z, p2y, -cyzEAl);
			set(p2x, p2y, cxyEAl);
			set(p2y, p2y, cyyEAl);
			set(p2z, p2y, cyzEAl);

			set(p1x, p2z, -cxzEAl);
			set(p1y, p2z, -cyzEAl);
			set(p1z, p2z, -czzEAl);
			set(p2x, p2z, cxzEAl);
			set(p2y, p2z, cyzEAl);
			set(p2z, p2z, czzEAl);
		}
	}

	sp::Vector force(const data::Force force_list[], int force_size)
	{
		sp::Vector F;
		for(int i = 0; i < force_size; i++)
		{
			auto& force = force_list[i];
			F[data_in[3 * force.p + 0]] = force.val.x;
			F[data_in[3 * force.p + 1]] = force.val.y;
			F[data_in[3 * force.p + 2]] = force.val.z;
		}
		return F;
	}

	void displace(Point3D displace_list[], const sp::Vector& dp)
	{
		for(int i = 0; i < sp::size; i++)
		{
			int out = data_out[i];
			auto& displace = displace_list[out / 3];
			switch(out % 3)
			{
			case 0:
				displace.x += dp[i];
				break;
			case 1:
				displace.y += dp[i];
				break;
			case 2:
				displace.z += dp[i];
				break;
			}
		}
	}

	void reaction(data::Force reaction_list[], const data::Fix fix_list[], int fix_size, const QVector<double>& reaction)
	{
		for(int i = 0; i < fix_size; i++)
		{
			reaction_list[i].p = fix_list[i].p;
		}

		auto data_reaction = reaction_list + fix_size - 1;
		for(int i = 0; i < reaction.size(); i++)
		{
			int out = data_out[sp::size + i];
			if(out / 3 != data_reaction->p)
				data_reaction--;
			switch(out % 3)
			{
			case 0:
				data_reaction->val.x = reaction[i];
				break;
			case 1:
				data_reaction->val.y = reaction[i];
				break;
			case 2:
				data_reaction->val.z = reaction[i];
				break;
			}
		}
	}
}
