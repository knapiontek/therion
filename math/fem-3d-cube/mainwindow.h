#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "input.h"

class Render;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();
	void keyReleaseEvent(QKeyEvent* event);

private:
	void dock_data();
	void dock_vector(const QString& name, const double vec[DOF_NO]);
	void dock_matrix(const QString& name, const double mx[DOF_NO][DOF_NO]);

private:
	QDockWidget* m_lastDockWidget;
	Render* m_render;
};

#endif // MAINWINDOW_H
