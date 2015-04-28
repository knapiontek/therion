#ifndef RENDER_H
#define RENDER_H

#include "input.h"

#include <QWidget>

struct View
{
	View()
		: display(DISPLAY_BOTH),
		  action(ACTION_ZOOM),
		  scale(0),
		  move_x(0),
		  move_y(0),
		  rotX(-1.3),
		  rotZ(.1)
	{
		sX = sin(rotX);
		cX = cos(rotX);
		sZ = sin(rotZ);
		cZ = cos(rotZ);
	}

	enum
	{
		DISPLAY_BOTH,
		DISPLAY_MESH,
		DISPLAY_SHIFT
	} display;

	enum
	{
		ACTION_ROTX,
		ACTION_ROTZ,
		ACTION_ZOOM
	} action;

	int scale;
	int move_x;
	int move_y;
	double rotX;
	double rotZ;
	double sX;
	double cX;
	double sZ;
	double cZ;
};

class Render : public QWidget
{
	Q_OBJECT
public:
	explicit Render(QWidget* parent = 0);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void keyEvent(QKeyEvent* event);

protected:
	void paintEvent(QPaintEvent* event);
	void wheelEvent(QWheelEvent* event);

private:
	void action(double delta, QPoint pos);

private:
	View m_view;
};

#endif // RENDER_H
