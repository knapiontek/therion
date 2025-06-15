#ifndef PAINTER_H
#define PAINTER_H

#include <QPainter>

struct Painter : public QPainter
{
    void drawArrow(const QPointF &begin, const QPointF &end)
    {
        qreal x = end.x() - begin.x();
        qreal y = end.y() - begin.y();

        qreal length = std::sqrt(x * x + y * y);

        qreal unitX = x / length;
        qreal unitY = y / length;

        drawLine(begin, end);

        qreal h = 8;
        qreal w = 2;

        if (length < 30) {
            h = h * length / 30;
            w = w * length / 30;
        }

        qreal midX = end.x() - h * unitX;
        qreal midY = end.y() - h * unitY;

        QPointF p1(midX + w * unitY, midY - w * unitX);
        QPointF p2(midX - w * unitY, midY + w * unitX);

        QPolygonF polygon;
        polygon << end << p1 << p2;
        drawPolygon(polygon);
    }
};

#endif // PAINTER_H
