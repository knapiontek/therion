#include "engine.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>

int main(int argc, char* argv[])
{
	engine_init();

	QApplication app(argc, argv);
	MainWindow mainWindow;

	mainWindow.show();
	return app.exec();
}
