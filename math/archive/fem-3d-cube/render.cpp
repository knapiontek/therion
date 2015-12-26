
#include "render.h"
#include "input.h"
#include "engine.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QWheelEvent>
#include <QSvgGenerator>

class PainterPath : public QPainterPath
{
public:
	PainterPath(View& view): m_view(view)
	{

	}
	void line(Point& p1, Point& p2)
	{
		QPointF rp1 = rot(p1);
		QPointF rp2 = rot(p2);

		QPointF sp1 = scale(rp1);
		QPointF sp2 = scale(rp2);

		moveTo(sp1);
		lineTo(sp2);
	}
	void force(Point& p1, Point& p2)
	{
		QPointF rp1 = rot(p1);
		QPointF rp2 = rot(p2);

		QPointF sp1 = scale(rp1);
		QPointF sp2 = scale(rp1 + .05 * rp2);

		moveTo(sp1);
		lineTo(sp2);

		auto l = ::sqrt(rp2.x() * rp2.x() + rp2.y() * rp2.y());
		if(m_view.scale * l > 150) // p2 long enough to draw arrow
		{
			QPointF height(10 * rp2 / l); // of arrow
			QPointF width(2 * rp2 / l); // of arrow

			lineTo(sp2.x() - height.x() - width.y(), sp2.y() + height.y() - width.x()); // 1st blade
			moveTo(sp2.x(), sp2.y());
			lineTo(sp2.x() - height.x() + width.y(), sp2.y() + height.y() + width.x()); // 2nd blade
		}
	}
	void fix(Point& p)
	{
		QPointF qp = rot(p);
		QPointF tp = scale(qp);
		addEllipse(tp.x() - 2, tp.y() - 2, 4, 4);
	}
private:
	QPointF rot(const Point& p)
	{
		// [1  0 0][ c s 0][x]
		// [0  c s][-s c 0][y]
		// [0 -s c][ 0 0 1][z]
		return QPointF(m_view.cZ * p.x + m_view.sZ * p.y,
			- m_view.cX * m_view.sZ * p.x + m_view.cX * m_view.cZ * p.y + m_view.sX * p.z);
	}
	QPointF scale(const QPointF& p)
	{
		return QPointF(m_view.move_x + (m_view.scale * p.x()), m_view.move_y - (m_view.scale * p.y()));
	}
private:
	View& m_view;
};

Render::Render(QWidget* parent) : QWidget(parent)
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

void Render::keyEvent(QKeyEvent* event)
{
	if(Qt::Key_D == event->key())
	{
		if(View::DISPLAY_BOTH == m_view.display)
		{
			m_view.display = View::DISPLAY_MESH;
		}
		else if(View::DISPLAY_MESH == m_view.display)
		{
			m_view.display = View::DISPLAY_SHIFT;
		}
		else if(View::DISPLAY_SHIFT == m_view.display)
		{
			m_view.display = View::DISPLAY_BOTH;
		}
	}
	else if(Qt::Key_X == event->key())
	{
		m_view.action = View::ACTION_ROTX;
	}
	else if(Qt::Key_Z == event->key())
	{
		m_view.action = View::ACTION_ROTZ;
	}
	else if(Qt::Key_M == event->key())
	{
		m_view.action = View::ACTION_ZOOM;
	}
	else if(Qt::Key_Up == event->key())
	{
		action(-120, QPoint(width() / 2, height() / 2));
	}
	else if(Qt::Key_Down == event->key())
	{
		action(120, QPoint(width() / 2, height() / 2));
	}
	else if(Qt::Key_S == event->key())
	{
		QSvgGenerator generator;
		generator.setFileName("screenshot.svg");
		generator.setSize(QSize(200, 200));
		generator.setViewBox(QRect(0, 0, 200, 200));
		generator.setTitle(tr("FEM Screenshot"));

		QPainter painter;
		painter.begin(&generator);
		painter.setRenderHint(QPainter::Antialiasing, true);
		paint(painter);
		painter.end();
	}
	repaint();
}

void Render::paintEvent(QPaintEvent*)
{
	if(0 == m_view.scale)
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
		m_view.scale = .8 * qMin(width() / len_x, height() / len_y);
		m_view.move_x = (.5 * width()) - (m_view.scale * mid_x);
		m_view.move_y = (.5 * height()) + (m_view.scale * mid_y);
	}

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	paint(painter);
}

void Render::wheelEvent(QWheelEvent* event)
{
	action(event->delta(), event->pos());
	repaint();
}

void Render::paint(QPainter& painter)
{
	PainterPath element_path(m_view);
	PainterPath shift_path(m_view);
	for(auto i = 0; i < ELEMENT_NO; i++)
	{
		auto& element = element_list[i];
		auto& data1 = data_list[element.n1];
		auto& data2 = data_list[element.n2];

		element_path.line(data1.node, data2.node);
		shift_path.line(data1.shift, data2.shift);
	}

	PainterPath force_path(m_view);
	PainterPath fix_path(m_view);
	for(auto i = 0; i < NODE_NO; i++)
	{
		auto& data = data_list[i];
		if(data.force.x + data.force.y + data.force.z != 0.0)
		{
			force_path.force(data.shift, data.force);
		}
		if(data.fix)
		{
			fix_path.fix(data.node);
		}
	}

	if(View::DISPLAY_BOTH == m_view.display || View::DISPLAY_MESH == m_view.display)
	{
		painter.setPen(QPen(Qt::gray, .5));
		painter.drawPath(element_path);
	}

	if(View::DISPLAY_BOTH == m_view.display || View::DISPLAY_SHIFT == m_view.display)
	{
		painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
		painter.drawPath(shift_path);

		painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
		painter.drawPath(force_path);
	}

	painter.setPen(QPen(Qt::darkGreen, 2, Qt::SolidLine));
	painter.drawPath(fix_path);
}

void Render::action(double delta, QPoint pos)
{
	if(View::ACTION_ROTX == m_view.action)
	{
		m_view.rotX += delta / 1200.0;
		m_view.sX = sin(m_view.rotX);
		m_view.cX = cos(m_view.rotX);
	}
	else if(View::ACTION_ROTZ == m_view.action)
	{
		m_view.rotZ += delta / 1200.0;
		m_view.sZ = sin(m_view.rotZ);
		m_view.cZ = cos(m_view.rotZ);
	}
	else if(View::ACTION_ZOOM == m_view.action)
	{
		Point point = { double(pos.x() - m_view.move_x) / m_view.scale, double(m_view.move_y - pos.y()) / m_view.scale, 0.0 };

		m_view.scale += delta / 10.0;
		m_view.scale = qMax(m_view.scale, width() / 100);

		m_view.move_x = pos.x() - (m_view.scale * point.x);
		m_view.move_y = pos.y() + (m_view.scale * point.y);
	}
}
