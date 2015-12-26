#include <QtGui>
#include <QDockWidget>
#include <QTableWidget>
#include "mainwindow.h"
#include "render.h"
#include "engine.h"

class DoubleTableWidgetItem : public QTableWidgetItem
{
public:
	DoubleTableWidgetItem(double val)
	{
		setText(QString::number(val));
		setTextAlignment(Qt::AlignRight);
	}
};

MainWindow::MainWindow() : lastDockWidget(0)
{
	auto render = new Render;
	setCentralWidget(render);
	dock_data();
	dock_matrix("K:", K);
	dock_matrix("L:", L);
	dock_matrix("U:", U);
	dock_vector("X:", X);
}

MainWindow::~MainWindow()
{

}

void MainWindow::dock_data()
{
	auto dockable = new QDockWidget("Input/Output", this);
	dockable->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	auto table = new QTableWidget;
	table->setRowCount(NODE_NO);
	table->setColumnCount(6);
	table->setHorizontalHeaderLabels(QStringList() << "X" << "Y" << "Force X" << "Force Y" << "X'" << "Y'");

	for(auto i = 0; i < NODE_NO; i++)
	{
		table->setVerticalHeaderItem(i, new DoubleTableWidgetItem(i));

		table->setItem(i, 0, new DoubleTableWidgetItem(data_list[i].node.x));
		table->setItem(i, 1, new DoubleTableWidgetItem(data_list[i].node.y));
		table->setItem(i, 2, new DoubleTableWidgetItem(data_list[i].force.x));
		table->setItem(i, 3, new DoubleTableWidgetItem(data_list[i].force.y));
		table->setItem(i, 4, new DoubleTableWidgetItem(data_list[i].shift.x));
		table->setItem(i, 5, new DoubleTableWidgetItem(data_list[i].shift.y));
	}

	dockable->setWidget(table);
	addDockWidget(Qt::RightDockWidgetArea, dockable);
	if(lastDockWidget)
		tabifyDockWidget(lastDockWidget, dockable);
	lastDockWidget = dockable;
}

void MainWindow::dock_vector(const QString& name, const double vec[DOF_NO])
{
	auto dockable = new QDockWidget(name, this);
	dockable->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	auto table = new QTableWidget;
	table->setRowCount(DOF_NO);
	table->setColumnCount(1);

	table->setHorizontalHeaderItem(0, new DoubleTableWidgetItem(0));

	for(auto i = 0; i < DOF_NO; i++)
	{
		table->setVerticalHeaderItem(i, new DoubleTableWidgetItem(i));
		table->setItem(i, 0, new DoubleTableWidgetItem(vec[i]));
	}

	dockable->setWidget(table);
	addDockWidget(Qt::RightDockWidgetArea, dockable);
	if(lastDockWidget)
		tabifyDockWidget(lastDockWidget, dockable);
	lastDockWidget = dockable;
}

void MainWindow::dock_matrix(const QString& name, const double mx[DOF_NO][DOF_NO])
{
	auto dockable = new QDockWidget(name, this);
	dockable->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	auto table = new QTableWidget;
	table->setRowCount(DOF_NO);
	table->setColumnCount(DOF_NO);

	for(auto i1 = 0; i1 < DOF_NO; i1++)
	{
		table->setVerticalHeaderItem(i1, new DoubleTableWidgetItem(i1));
		table->setHorizontalHeaderItem(i1, new DoubleTableWidgetItem(i1));

		for(auto i2 = 0; i2 < DOF_NO; i2++)
		{
			table->setItem(i1, i2, new DoubleTableWidgetItem(mx[i1][i2]));
		}
	}

	dockable->setWidget(table);
	addDockWidget(Qt::RightDockWidgetArea, dockable);
	if(lastDockWidget)
		tabifyDockWidget(lastDockWidget, dockable);
	lastDockWidget = dockable;
}
