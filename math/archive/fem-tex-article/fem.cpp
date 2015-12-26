#include <QApplication>
#include <QRegularExpression>

#include "paint.hpp"
#include "data.cpp"
#include "populate.cpp"
#include "calculate.cpp"
#include "rotate.cpp"

Point2D west(const Point3D& p)
{
	return Point2D
	(
		- 4 + p.x,
		2 + p.y
	);
}

Point2D east(const Point2D& p)
{
	return Point2D
	(
		3 + p.x,
		2 + p.y
	);
}

void store_unit_vector(Paint& paint, const Point3D& p0)
{
	Point3D px = { 1 + p0.x, p0.y, p0.z };
	Point3D py = { p0.x, 1 + p0.y, p0.z };
	Point3D pz = { p0.x, p0.y, 3 + p0.z };

	Point2D p0r = rotate(p0);
	Point2D pxr = rotate(px);
	Point2D pyr = rotate(py);
	Point2D pzr = rotate(pz);

	Point2D p0m = east(p0r);
	Point2D pxm = east(pxr);
	Point2D pym = east(pyr);
	Point2D pzm = east(pzr);

	paint.line(p0m, pxm, Paint::ARROW);
	paint.text("$x$", pxm, Paint::BELOW);

	paint.line(p0m, pym, Paint::ARROW);
	paint.text("$y$", pym, Paint::LEFT);

	paint.line(p0m, pzm, Paint::ARROW);
	paint.text("$z$", pzm, Paint::LEFT);
}

void store_rotated_elements(Paint& paint)
{
	for(int i = 0; i < element_no; i++)
	{
		const Element& element = element_list[i];
		const Point3D& p1 = point_list[element.p1];
		const Point3D& p2 = point_list[element.p2];

		Point2D p1r = rotate(p1);
		Point2D p2r = rotate(p2);

		Point2D p1m = east(p1r);
		Point2D p2m = east(p2r);

		paint.line(p1m, p2m);
	}
}

void store_figure_final()
{
	Paint paint("final");

	const Point3D p = { -4, 0, 0 };
	store_unit_vector(paint, p);
	store_rotated_elements(paint);

	// draw output
	for(int i = 0; i < element_no; i++)
	{
		const Element& element = element_list[i];
		const Point3D& p1 = output_list[element.p1];
		const Point3D& p2 = output_list[element.p2];

		Point2D p1r = rotate(p1);
		Point2D p2r = rotate(p2);

		Point2D p1m = east(p1r);
		Point2D p2m = east(p2r);

		paint.line(p1m, p2m, Paint::DASH);
	}

	// move labels beside points
	Paint::Style style_list[point_no] = {
		Paint::BELOW | Paint::RIGHT,
		Paint::LEFT,
		Paint::BELOW,
		Paint::BELOW | Paint::LEFT
	};

	// draw forces and reactions (at fixed displacements) and labels
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& p1 = output_list[i];
		const Point3D& p2 = force_list[i];
		const Point3D& fix = fix_list[i];
		Paint::Style style = style_list[i];

		Point2D p1r = rotate(p1);
		Point2D p2r = rotate(p2);

		Point2D p1m = east(p1r);
		Point2D p2m = east(p1r + (.03 * p2r)); // scaling

		// mark fixed displacement
		if(fix.x + fix.y + fix.z)
			paint.dot(p1m, Paint::RED);

		if(p2.x + p2.y + p2.z)
			paint.line(p1m, p2m, Paint::ARROW | Paint::RED);

		paint.text(QString("$p_%1$").arg(i), p1m, style);
	}

	// legend
	Point2D p0(4, 5);
	Point2D p1(1, 0);

	paint.line(p0, p0+p1);
	paint.text("original geometry", p0+p1, Paint::RIGHT);

	p0.y -= .5;
	paint.line(p0, p0+p1, Paint::DASH);
	paint.text("geometry deformation", p0+p1, Paint::RIGHT);

	p0.y -= .5;
	paint.line(p0, p0+p1, Paint::ARROW | Paint::RED);
	paint.text("applied external force", p0+p1, Paint::RIGHT);

	p0.y -= .5;
	paint.line(p0, p0+p1, Paint::ARROW | Paint::RED);
	paint.text("reactions in fixed points", p0+p1, Paint::RIGHT);
	paint.dot(p0, Paint::RED);
}

void store_figure_breakdown()
{
	Paint paint("breakdown");

	const Point3D p0 = { -6, 0, 0 };
	store_unit_vector(paint, p0);

	const Point2D move_element_list[element_no] = {
		Point2D(0, .3), // 0
		Point2D(.4, .4), // 1
		Point2D(.4, 0), // 2
		Point2D(-.1,-.1), // 3
		Point2D(-.4, 0), // 4
		Point2D(0,-.4) // 5
	};

	Paint::Style style_element_list[element_no] = {
		Paint::RIGHT, // 0
		Paint::LEFT, // 1
		Paint::ABOVE, // 2
		Paint::RIGHT, // 3
		Paint::LEFT, // 4
		Paint::BELOW // 5
	};

	// draw elements
	for(int i = 0; i < element_no; i++)
	{
		const Element& element = element_list[i];
		Point3D p1 = point_list[element.p1];
		Point3D p2 = point_list[element.p2];
		const Point2D& move_element = move_element_list[i];
		Paint::Style style = style_element_list[i];

		Point2D p1r = rotate(p1);
		Point2D p2r = rotate(p2);

		Point2D p1m = east(p1r) + move_element;
		Point2D p2m = east(p2r) + move_element;

		paint.line(p1m, p2m, Paint::RED | Paint::THICK);
		paint.dot(p1m);
		paint.dot(p2m);

		paint.text(QString("$e_%1$").arg(i), (p1m+p2m)/2, style);
	}

	// move labels beside points
	const Point2D move_point_list[point_no] = {
		Point2D(.15, 0),
		Point2D(0, .6),
		Point2D(.7, 0),
		Point2D(-.5, -.5)
	};

	// point labels
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& p = point_list[i];
		const Point2D& move_point = move_point_list[i];

		Point2D pr = rotate(p);
		Point2D pm = east(pr) + move_point;

		paint.text(QString("$p_%1$").arg(i), pm);
	}
}

void store_figure_displacement_force()
{
	double x0, y0, dx0, dy0, x1, y1, dx1, dy1, fx, fy;

	Paint paint("displacement-force");

	x0 = .7;
	y0 = .7;
	dx0 = 2;
	dy0 = 1.5;

	x1 = 3;
	y1 = 5;
	dx1 = 3;
	dy1 = 1;

	paint.text("a) Displacements", Point2D(1, 7), Paint::RIGHT);

	// global arrows
	paint.path(Point2D(0, 7), Point2D(0, 0), Point2D(7, 0), Paint::DARROW);
	paint.text("$x$", Point2D(7, 0), Paint::ABOVE);
	paint.text("$y$", Point2D(0, 7), Paint::LEFT);

	// auxyliary lines and local arrows x0, y0
	paint.path(Point2D(x0, y0+dy0), Point2D(x0+dx0, y0+dy0), Point2D(x0+dx0, y0), Paint::DASH);
	paint.path(Point2D(x0, y0+dy0), Point2D(x0, y0), Point2D(x0+dx0, y0), Paint::DARROW);
	paint.text("$\\Delta x_0$", Point2D(x0+(dx0/2), y0), Paint::ABOVE);
	paint.text("$\\Delta y_0$", Point2D(x0, y0+(dy0/2)), Paint::LEFT);
	paint.line(Point2D(x0, y0), Point2D(x0+dx0, y0+dy0), Paint::ARROW);
	paint.text("$\\Delta p_0$", Point2D(x0+(dx0/2), y0+(dy0/2)), Paint::ABOVE);
	paint.text("$(x_0,y_0)$", Point2D(x0, y0), Paint::BELOW);

	// auxyliary lines and local arrows x1, y1
	paint.path(Point2D(x1, y1+dy1), Point2D(x1+dx1, y1+dy1), Point2D(x1+dx1, y1), Paint::DASH);
	paint.path(Point2D(x1, y1+dy1), Point2D(x1, y1), Point2D(x1+dx1, y1), Paint::DARROW);
	paint.text("$\\Delta x_1$", Point2D(x1+(dx1/2), y1), Paint::ABOVE | Paint::RIGHT);
	paint.text("$\\Delta y_1$", Point2D(x1, y1+(dy1/2)), Paint::LEFT);
	paint.line(Point2D(x1, y1), Point2D(x1+dx1, y1+dy1), Paint::ARROW);
	paint.text("$\\Delta p_1$", Point2D(x1+(dx1/2), y1+(dy1/2)), Paint::ABOVE);
	paint.text("$(x_1,y_1)$", Point2D(x1, y1), Paint::BELOW | Paint::RIGHT);

	// original element
	paint.line(Point2D(x0, y0), Point2D(x1, y1), Paint::THICK | Paint::RED);
	paint.text("$L$", Point2D((x0+x1)/2, (y0+y1)/2), Paint::THICK | Paint::RED | Paint::ABOVE | Paint::LEFT);
	paint.dot(Point2D(x0, y0));
	paint.dot(Point2D(x1, y1));

	// displaced element
	paint.line(Point2D(x0+dx0, y0+dy0), Point2D(x1+dx1, y1+dy1), Paint::THICK | Paint::RED | Paint::DASH);
	paint.text("$L\'$", Point2D((x0+dx0+x1+dx1)/2, (y0+dy0+y1+dy1)/2), Paint::RED | Paint::BELOW | Paint::RIGHT);
	paint.dot(Point2D(x0+dx0, y0+dy0));
	paint.dot(Point2D(x1+dx1, y1+dy1));

	fx = 1.5;
	fy = 2;

	x0 = 10;
	y0 = 2.5;
	x1 = 12;
	y1 = y0 + (x1-x0)*fy/fx; // forces in line to element

	paint.text("b) Forces", Point2D(8.5, 7), Paint::RIGHT);

	// global arrows
	paint.path(Point2D(7.5, 7), Point2D(7.5, 0), Point2D(14.5, 0), Paint::DARROW);
	paint.text("$x$", Point2D(14.5, 0), Paint::ABOVE);
	paint.text("$y$", Point2D(7.5, 7), Paint::LEFT);

	// auxyliary lines and local arrows x0, y0
	paint.path(Point2D(x0-fx, y0), Point2D(x0-fx, y0-fy), Point2D(x0, y0-fy), Paint::DASH);
	paint.path(Point2D(x0-fx, y0), Point2D(x0, y0), Point2D(x0, y0-fy), Paint::DARROW);
	paint.text("$f_{x_0}$", Point2D(x0-(fx/2), y0), Paint::ABOVE);
	paint.text("$f_{y_0}$", Point2D(x0, y0-(fy/2)), Paint::RIGHT);
	paint.line(Point2D(x0, y0), Point2D(x0-fx, y0-fy), Paint::ARROW);
	paint.text("$f_0$", Point2D(x0-(fx/2), y0-(fy/2)), Paint::BELOW);
	paint.text("$(x_0,y_0)$", Point2D(x0, y0), Paint::RIGHT);

	// auxyliary lines and local arrows x1, y1
	paint.path(Point2D(x1, y1+fy), Point2D(x1+fx, y1+fy), Point2D(x1+fx, y1), Paint::DASH);
	paint.path(Point2D(x1, y1+fy), Point2D(x1, y1), Point2D(x1+fx, y1), Paint::DARROW);
	paint.text("$f_{x_1}$", Point2D(x1+(fx/2), y1), Paint::ABOVE);
	paint.text("$f_{y_1}$", Point2D(x1, y1+(fy/2)), Paint::LEFT);
	paint.line(Point2D(x1, y1), Point2D(x1+fx, y1+fy), Paint::ARROW);
	paint.text("$f_1$", Point2D(x1+(fx/2), y1+(fy/2)), Paint::ABOVE);
	paint.text("$(x_1,y_1)$", Point2D(x1, y1), Paint::BELOW | Paint::RIGHT);

	// element
	paint.line(Point2D(x0, y0), Point2D(x1, y1), Paint::THICK | Paint::RED | Paint::DASH);
	paint.text("$L\'$", Point2D((x0+x1)/2, (y0+y1)/2), Paint::RED | Paint::ABOVE | Paint::LEFT);
	paint.dot(Point2D(x0, y0));
	paint.dot(Point2D(x1, y1));
}

void store_figure_displacement_details()
{
	Paint paint("displacement-details");

	double x = 1;
	double y = 1;
	double dx = 5;
	double dy = 3;
	double cx = dx * dx * dx / (dx * dx + dy * dy); // simplified double cos congruence
	double cy = cx * dy / dx; // tan congruence
	double ax = cx / 15;
	double ay = cy / 15;

	// global arrows
	Point2D xaxis(7, 0);
	Point2D yaxis(0, 5);
	paint.path(yaxis, Point2D(0, 0), xaxis, Paint::DARROW);
	paint.text("$x$", xaxis, Paint::ABOVE);
	paint.text("$y$", yaxis, Paint::RIGHT);

	// auxyliary lines and local arrows
	paint.path(Point2D(x, y+dy), Point2D(x+dx, y+dy), Point2D(x+dx, y), Paint::DASH);
	paint.path(Point2D(x, y+dy), Point2D(x, y), Point2D(x+dx, y), Paint::DARROW);
	paint.text("$\\Delta x_0$", Point2D(x+(dx/2), y), Paint::BELOW);
	paint.text("$\\Delta y_0$", Point2D(x, y+(dy/2)), Paint::LEFT);

	// displacement
	paint.line(Point2D(x, y), Point2D(x+dx, y+dy), Paint::RED | Paint::ARROW);
	paint.text("$\\Delta p_0$", Point2D(x+(dx/2), y+(dy/2)), Paint::RED | Paint::ABOVE);
	paint.line(Point2D(x+dx, y), Point2D(x+cx, y+cy), Paint::RED | Paint::DASH);

	// labels
	paint.text("$\\theta_x$", Point2D(x+1, y+.3));
	paint.text("$\\theta_y$", Point2D(x+dx-.6, y+.3));
	paint.text("$\\theta_x$", Point2D(x+dx-.2, y+0.8));
	paint.text("$\\theta_y$", Point2D(x+dx-.2, y+dy-.5));

	// point
	paint.dot(Point2D(x, y));
	paint.text("$(x_0,y_0)$", Point2D(x, y), Paint::BELOW);

	// right angle
	paint.path(Point2D(x+cx-ax, y+cy-ay), Point2D(x+cx-ax+ay, y+cy-ay-ax), Point2D(x+cx+ay, y+cy-ax));
}

void store_figure_point0_forces()
{
	Paint paint("point0-forces");

	Point3D px1 = { -1.5, 0, 0 };
	Point3D px2 = { 1.5, 0, 0 };
	Point3D py1 = { 0, 1.5, 0 };
	Point3D py2 = { 0, -1.5, 0 };
	Point3D pz1 = { 0, 0, -5.5 };
	Point3D pz2 = { 0, 0, 5.5 };

	Point2D px1r = rotate(px1);
	Point2D px2r = rotate(px2);
	Point2D py1r = rotate(py1);
	Point2D py2r = rotate(py2);
	Point2D pz1r = rotate(pz1);
	Point2D pz2r = rotate(pz2);

	Point2D px1m = east(px1r);
	Point2D px2m = east(px2r);
	Point2D py1m = east(py1r);
	Point2D py2m = east(py2r);
	Point2D pz1m = east(pz1r);
	Point2D pz2m = east(pz2r);

	paint.line(px1m, px2m, Paint::DARROW | Paint::RED);
	paint.text("$R_x$", px1m + Point2D(-.2, 0));
	paint.text("$S_x$", px2m + Point2D(.2, 0));

	paint.line(py1m, py2m, Paint::DARROW | Paint::RED);
	paint.text("$R_y$", py1m + Point2D(0, .2));
	paint.text("$S_y$", py2m + Point2D(0, -.2));

	paint.line(pz1m, pz2m, Paint::DARROW | Paint::RED);
	paint.text("$R_z$", pz1m + Point2D(.1, .1));
	paint.text("$S_z$", pz2m + Point2D(-.14, -.14));

	paint.text("$p_0$", Point2D(3.3, 1.7));
}

void store_figure_fix_displacement()
{
	Paint paint("fix-displacement");

	const Point3D p0 = { -4, 0, 0 };
	store_unit_vector(paint, p0);
	store_rotated_elements(paint);

	Paint::Style style_list[point_no] = {
		Paint::ABOVE | Paint::RIGHT,
		Paint::ABOVE,
		Paint::BELOW,
		Paint::BELOW | Paint::LEFT
	};

	// draw fixed displacements
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& p = point_list[i];
		const Point3D& fix = fix_list[i];
		Paint::Style style = style_list[i];

		Point2D pr = rotate(p);
		Point2D pm = east(pr);

		if(fix.x + fix.y + fix.z)
			paint.dot(pm, Paint::RED);

		paint.text(QString("$p_%1$").arg(i), pm, style);
	}

	Point2D p(3, 3.5);
	paint.dot(p, Paint::RED);
	paint.text("Fixed Point Displacement", p, Paint::RIGHT);
}

void store_figure_lu_direction()
{
	double l = 2; // lenght
	double m = .2; // margin
	double s = 3; // shift

	Paint paint("lu-direction");

	paint.path(Point2D(0, -m), Point2D(-m, -m), Point2D(-m, l+m), Point2D(0, l+m), Paint::THICK);
	paint.line(Point2D(0, l), Point2D(l, 0), Paint::ARROW | Paint::DASH);
	paint.line(Point2D(0, l), Point2D(0, 0), Paint::ARROW | Paint::DASH);
	paint.line(Point2D(l/2, l/2), Point2D(l/2, 0), Paint::ARROW | Paint::DASH);
	paint.path(Point2D(l, -m), Point2D(l+m, -m), Point2D(l+m, l+m), Point2D(l, l+m), Paint::THICK);

	paint.path(Point2D(s, -m), Point2D(s-m, -m), Point2D(s-m, l+m), Point2D(s, l+m), Paint::THICK);
	paint.line(Point2D(s, l), Point2D(s+l, 0), Paint::ARROW | Paint::DASH);
	paint.line(Point2D(s, l), Point2D(s+l, l), Paint::ARROW | Paint::DASH);
	paint.line(Point2D(s+l/2, l/2), Point2D(s+l, l/2), Paint::ARROW | Paint::DASH);
	paint.path(Point2D(s+l, -m), Point2D(s+l+m, -m), Point2D(s+l+m, l+m), Point2D(s+l, l+m), Paint::THICK);
}

void store_figure_rotate_point()
{
	Paint paint("rotate-point");

	double radius = 5;
	double alpha0 = pi/9;
	double alpha1 = pi/3.5;

	Point2D p0 = Point2D::polar(radius, alpha0);
	Point2D p1 = Point2D::polar(radius, alpha1);

	// global arrows
	Point2D xaxis(5.5, 0);
	Point2D yaxis(0, 4.5);
	paint.path(yaxis, Point2D(0, 0), xaxis, Paint::DARROW);
	paint.text("$x$", xaxis, Paint::ABOVE);
	paint.text("$y$", yaxis, Paint::RIGHT);

	// auxyliary lines
	paint.path(Point2D(0, p0.y), p0, Point2D(p0.x, 0), Paint::DASH);
	paint.path(Point2D(0, p1.y), p1, Point2D(p1.x, 0), Paint::DASH);

	// original line
	paint.line(Point2D(0, 0), p0, Paint::THICK | Paint::RED);
	paint.text("$r$", p0/2, Paint::ABOVE | Paint::RED);
	paint.text("$(x_0,y_0)$", p0, Paint::RIGHT);
	paint.dot(p0);

	// rotated line
	paint.line(Point2D(0, 0), p1, Paint::THICK | Paint::RED | Paint::DASH);
	paint.text("$r$", p1/2, Paint::ABOVE | Paint::RED);
	paint.text("$(x_1,y_1)$", p1, Paint::RIGHT);
	paint.dot(p1);

	// angles
	paint.arc(Point2D::polar(1.5, 0), 1.5, 0, alpha0);
	paint.text("$\\alpha_0$", Point2D::polar(2, alpha0/2));
	paint.arc(Point2D::polar(2, alpha0), 2, alpha0, alpha1, Paint::ARROW);
	paint.text("$\\alpha_1$", Point2D::polar(1.5, (alpha1+alpha0)/2));
}

void store_figure_rotate_truss()
{
	Paint paint("rotate-truss");

	paint.text("a) Original", Point2D(-4, 4));

	// unit vectors
	Point2D py(-2, 3.5);
	Point2D pz(-2, 2.5);
	Point2D px(-1, 2.5);
	paint.path(py, pz, px, Paint::DARROW);
	paint.text("$y$", py, Paint::LEFT);
	paint.text("$z=0$", pz, Paint::BELOW);
	paint.text("$x$", px, Paint::BELOW);

	// draw elements
	for(int i = 0; i < element_no; i++)
	{
		const Element& element = element_list[i];
		const Point3D& p1 = point_list[element.p1];
		const Point3D& p2 = point_list[element.p2];

		Point2D p1m = west(p1);
		Point2D p2m = west(p2);

		paint.line(p1m, p2m);
	}

	Paint::Style style_original_list[point_no] = {
		Paint::LEFT,
		Paint::LEFT,
		Paint::BELOW,
		Paint::BELOW
	};

	// draw point labels
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& p = point_list[i];
		Paint::Style style = style_original_list[i];

		Point2D pm = west(p);

		paint.text(QString("$p_%1$").arg(i), pm, style);
	}

	paint.text("b) Rotated", Point2D(3, 4));

	const Point3D p0 = { 2, .6, 0 };
	store_unit_vector(paint, p0);
	store_rotated_elements(paint);

	Paint::Style style_rotated_list[point_no] = {
		Paint::ABOVE | Paint::RIGHT,
		Paint::LEFT,
		Paint::BELOW,
		Paint::BELOW
	};

	// draw point labels
	for(int i = 0; i < point_no; i++)
	{
		const Point3D& p = point_list[i];
		Paint::Style style = style_rotated_list[i];

		Point2D pr = rotate(p);
		Point2D pm = east(pr);

		paint.text(QString("$p_%1'$").arg(i), pm, style);
	}
}

const QString equation_nil = "equation/nil.tex";
const QString equation_fix_displacement = "equation/fix-displacement.tex";
const QString equation_full = "equation/full.tex";

// storing K, dP and F in a tex matrix format
void store_equation(const QString& filename)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream stream(&file);
	stream.setRealNumberPrecision(4);
	stream << "\\begin{equation}\n\\resizebox{.93 \\textwidth}{!} {$\n\\begin{bmatrix}\n";
	for(int i1 = 0; i1 < dof_no; i1++)
	{
		if(i1) stream << " \\\\\n";
		for(int i2 = 0; i2 < dof_no; i2++)
		{
			if(i2) stream << " & ";
			if(0 == K[i1][i2]) stream << ".";
			else if(-1 == K[i1][i2]) stream << "\\color{blue}{" << K[i1][i2] << "}";
			else stream << K[i1][i2];
		}
	}
	stream << "\n\\end{bmatrix}\n\\begin{bmatrix}\n";
	for(int i = 0; i < dof_no; i++)
	{
		if(i) stream << " \\\\\n";
		if(0 == dP[i]) stream << ".";
		else if(-1 == K[i][i]) stream << "\\color{blue}{" << dP[i] << "}";
		else stream << dP[i];
	}
	stream << "\n\\end{bmatrix}=\n\\begin{bmatrix}\n";
	for(int i = 0; i < dof_no; i++)
	{
		if(i) stream << " \\\\\n";
		if(0 != F[i]) stream << F[i];
		else stream << ".";
	}
	stream << "\n\\end{bmatrix}\n$}\n\\end{equation}";
	file.close();
}

#define ERASE(arr) ::memset((arr), 0, sizeof(arr));

void main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	populate_equation();
	store_equation(equation_fix_displacement);

	calculate_equation();
	store_equation(equation_full);

	store_figure_final();
	store_figure_breakdown();
	store_figure_displacement_force();
	store_figure_displacement_details();
	store_figure_point0_forces();
	store_figure_fix_displacement();
	store_figure_lu_direction();
	store_figure_rotate_point();
	store_figure_rotate_truss();

	ERASE(K);
	ERASE(dP);
	ERASE(F);
	ERASE(fix_list);
	ERASE(force_list);
	populate_equation();
	store_equation(equation_nil);

	app.exit();
}
