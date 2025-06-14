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

        qreal midX = end.x() - 8 * unitX;
        qreal midY = end.y() - 8 * unitY;

        QPointF p1(midX + 2 * unitY, midY - 2 * unitX);
        QPointF p2(midX - 2 * unitY, midY + 2 * unitX);

        drawLine(begin, end);
        QPolygonF polygon;
        polygon << end << p1 << p2;
        drawPolygon(polygon);
    }
};

#endif // PAINTER_H
