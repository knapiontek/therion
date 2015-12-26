
#include <QApplication>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QVector>
#include <QTime>
#include <QDebug>

const double pi = 4.0 * std::atan(1.0);

#include <math.h>

#include "point.hpp"
#include "paint.hpp"
#include "sparse.hpp"
#include "data.hpp"
#include "clip.hpp"
#include "convert.hpp"

#include "tetrahedron.hpp"
#include "solve-tetrahedron.hpp"
#include "pipe.hpp"
#include "solve-pipe.hpp"
#include "test.hpp"
#include "doc.hpp"
#include "gen-pipe.hpp"

void main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	test::steepest_descent();
	test::conjagate_gradients();
	test::clip_triangles();
	test::intersection();

	solve_tetrahedron::run();
	solve_pipe::run();

	doc::store_figure_quadratic_form();
	doc::store_figure_levels();

	//gen_pipe::run();

	app.exit();
}
