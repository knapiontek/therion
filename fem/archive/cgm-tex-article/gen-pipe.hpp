
namespace gen_pipe
{
	void run()
	{
		const QString point_format = "\n\t\t{ %1, %2, %3 }, // %4";
		const QString element_format = "\n\t\t{ %1, %2 },";

		QFile file("pipe.hpp");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream stream(&file);
		stream.setRealNumberPrecision(4);

		const int z_max = 6;
		const int rot_max = 24;
		const double z_step = 1;
		const double rot_step = 2.0 * pi / rot_max;
		const double r1 = 2.0;
		const double r2 = 2.5;

		stream << "namespace pipe"
				  "\n{"
				  "\n\tconst Point3D point_list[]"
				  "\n\t{";

		for(int z = 0; z < z_max; z++)
		{
			for(int rot = 0; rot < rot_max; rot++)
			{
				const int rot_no = 2;

				double angle = rot * rot_step;
				double cos = std::cos(angle);
				double sin = std::sin(angle);
				double length = z * z_step;

				stream << QString(point_format)
						.arg(r1 * cos)
						.arg(r1 * sin)
						.arg(length)
						.arg((z * rot_max + rot) * rot_no);
				stream << QString(point_format)
						.arg(r2 * cos)
						.arg(r2 * sin)
						.arg(length)
						.arg((z * rot_max + rot) * rot_no + 1);
			}
		}

		stream << "\n\t};"
				  "\n"
				  "\n\tconst data::Element element_list[]"
				  "\n\t{";

		for(int z = 0; z < z_max; z++)
		{
			for(int rot = 0; rot < rot_max; rot++)
			{
				const int rot_no = 2;

				int p1 = (z * rot_max + rot) * rot_no;
				int p2 = (rot < rot_max - 1)
						? (z * rot_max + rot + 1) * rot_no
						: z * rot_max * rot_no;
				int p3 = ((z - 1) * rot_max + rot) * rot_no;

				stream << QString(element_format)
						.arg(p1)
						.arg(p1 + 1);
				stream << QString(element_format)
						.arg(p1)
						.arg(p2 + 1);
				stream << QString(element_format)
						.arg(p1)
						.arg(p2);
				stream << QString(element_format)
						.arg(p1 + 1)
						.arg(p2 + 1);

				if(z)
				{
					stream << QString(element_format)
							.arg(p1)
							.arg(p3);
					stream << QString(element_format)
							.arg(p1 + 1)
							.arg(p3 + 1);
					stream << QString(element_format)
							.arg(p1)
							.arg(p3 + 1);
					stream << QString(element_format)
							.arg(p2)
							.arg(p3);
					stream << QString(element_format)
							.arg(p2 + 1)
							.arg(p3 + 1);
				}
			}
		}

		stream << "\n\t};\n\n\tconst data::Fix fix_list[]"
				  "\n\t{";

		for(int rot = 0; rot < rot_max; rot++)
		{
			const int rot_no = 2;

			stream << QString("\n\t\t{ %1, data::X | data::Y | data::Z },")
						.arg(rot * rot_no + 1);
		}

		stream << "\n\t};"
				  "\n\n\tconst data::Force force_list[]"
				  "\n\t{"
				  "\n\t\t{ 93, { 0.0, -132.0, 0.0 } },"
				  "\n\t\t{ 95, { 0.0, -72.0, 0.0 } }"
				  "\n\t};"
				  "\n}"
				  "\n";
	}
}
