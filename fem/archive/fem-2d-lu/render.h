#ifndef RENDER_H
#define RENDER_H

#include "input.h"

#include <QWidget>

class Render : public QWidget
{
	Q_OBJECT
public:
	explicit Render(QWidget* parent = 0);
	QSize minimumSizeHint() const;
	QSize sizeHint() const;

protected:
	void paintEvent(QPaintEvent* event);
	void wheelEvent(QWheelEvent* event);

private:
	int scale;
	int move_x;
	int move_y;
};

#endif // RENDER_H
