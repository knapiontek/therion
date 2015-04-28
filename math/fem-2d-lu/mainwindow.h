#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "input.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

private:
	void dock_data();
	void dock_vector(const QString& name, const double vec[DOF_NO]);
	void dock_matrix(const QString& name, const double mx[DOF_NO][DOF_NO]);

private:
	QDockWidget* lastDockWidget;
};

#endif // MAINWINDOW_H
