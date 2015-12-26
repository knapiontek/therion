
#include "render.h"
#include "input.h"
#include "engine.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QWheelEvent>

class PainterPath : public QPainterPath
{
public:
	PainterPath(int scale, int move_x, int move_y)
	{
		this->scale = scale;
		this->move_x = move_x;
		this->move_y = move_y;
	}
	void line(Point& p1, Point& p2)
	{
		moveTo(tx(p1.x), ty(p1.y));
		lineTo(tx(p2.x), ty(p2.y));
	}
	void force(Point& p1, Point& p2)
	{
		auto l = ::sqrt(p2.x * p2.x + p2.y * p2.y);
		Point tp1 = { tx(p1.x), ty(p1.y) };
		Point tp2 = { tx(p1.x + .05 * p2.x), ty(p1.y + .05 * p2.y) };

		Point height = { 10 * p2.x / l, - 10 * p2.y / l }; // of arrow
		Point width = { 2 * p2.x / l, - 2 * p2.y / l }; // of arrow

		moveTo(tp1.x, tp1.y);
		lineTo(tp2.x, tp2.y);
		if(scale * l > 150) // p2 long enough to draw arrow
		{
			lineTo(tp2.x - height.x + width.y, tp2.y - height.y - width.x); // 1st blade
			moveTo(tp2.x, tp2.y);
			lineTo(tp2.x - height.x - width.y, tp2.y - height.y + width.x); // 2nd blade
		}
	}
	void fix(Point& p1, Point& p2)
	{
		Point tp1 = { tx(p1.x), ty(p1.y) };
		addEllipse(tp1.x - 2, tp1.y - 2, 4, 4);

		auto l = ::sqrt(p2.x * p2.x + p2.y * p2.y);
		if(l > 0)
		{
			Point np2 = { 10 * p2.x / l, 10 * p2.y / l }; // slip line
			moveTo(tp1.x - np2.x, tp1.y - np2.y);
			lineTo(tp1.x + np2.x, tp1.y + np2.y);
		}
	}
private:
	double tx(double value)
	{
		return move_x + (scale * value);
	}
	double ty(double value)
	{
		return move_y - (scale * value);
	}
private:
	int scale;
	int move_x;
	int move_y;
};

Render::Render(QWidget* parent) : QWidget(parent), scale(0), move_x(0), move_y(0)
{
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);
}

QSize Render::minimumSizeHint() const
{
	return QSize(100, 100);
}

QSize Render::sizeHint() const
{
	return QSize(800, 600);
}

void Render::paintEvent(QPaintEvent*)
{
	if(0 == scale)
	{
		Point scope[2];
		scope[0] = scope[1]	= data_list[0].node;
		for(auto i = 1; i < NODE_NO; i++)
		{
			auto& node = data_list[i].node;
			scope[0].x = qMin(scope[0].x, node.x);
			scope[0].y = qMin(scope[0].y, node.y);
			scope[1].x = qMax(scope[1].x, node.x);
			scope[1].y = qMax(scope[1].y, node.y);
		}

		auto mid_x = .5 * (scope[0].x + scope[1].x);
		auto mid_y = .5 * (scope[0].y + scope[1].y);
		auto len_x = scope[1].x - scope[0].x;
		auto len_y = scope[1].y - scope[0].y;
		scale = .8 * qMin(width() / len_x, height() / len_y);
		move_x = (.5 * width()) - (scale * mid_x);
		move_y = (.5 * height()) + (scale * mid_y);
	}

	PainterPath element_path(scale, move_x, move_y);
	PainterPath shift_path(scale, move_x, move_y);
	for(auto i = 0; i < ELEMENT_NO; i++)
	{
		auto& element = element_list[i];
		auto& data1 = data_list[element.n1];
		auto& data2 = data_list[element.n2];

		element_path.line(data1.node, data2.node);
		shift_path.line(data1.shift, data2.shift);
	}

	PainterPath force_path(scale, move_x, move_y);
	PainterPath fix_path(scale, move_x, move_y);
	for(auto i = 0; i < NODE_NO; i++)
	{
		auto& data = data_list[i];

		if(data.force.x + data.force.y != 0.0)
		{
			force_path.force(data.shift, data.force);
		}
		if(data.fix.x * data.fix.y == 0.0)
		{
			fix_path.fix(data.node, data.fix);
		}
	}

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	painter.setPen(QPen(Qt::gray, .5));
	painter.drawPath(element_path);

	painter.setPen(QPen(Qt::green, 1, Qt::DashLine));
	painter.drawPath(shift_path);

	painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
	painter.drawPath(force_path);

	painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
	painter.drawPath(fix_path);
}

void Render::wheelEvent(QWheelEvent* event)
{
	auto pos = event->pos();
	Point point = { double(pos.x() - move_x) / scale, double(move_y - pos.y()) / scale };

	scale += event->delta() / 10.0;
	scale = qMax(scale, width() / 100);

	move_x = pos.x() - (scale * point.x);
	move_y = pos.y() + (scale * point.y);

	repaint();
}
