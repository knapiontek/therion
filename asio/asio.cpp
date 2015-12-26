
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
#include "diagram.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	draw_thread_switch_context();
	app.exit();
	return 0;
}
