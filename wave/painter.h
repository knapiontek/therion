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

        if (length > 30) {
            qreal midX = end.x() - 8 * unitX;
            qreal midY = end.y() - 8 * unitY;

            QPointF p1(midX + 2 * unitY, midY - 2 * unitX);
            QPointF p2(midX - 2 * unitY, midY + 2 * unitX);

            QPolygonF polygon;
            polygon << end << p1 << p2;
            drawPolygon(polygon);
        } else if (length > 10) {
            qreal midX = end.x() - 4 * unitX;
            qreal midY = end.y() - 4 * unitY;

            QPointF p1(midX + 1 * unitY, midY - 1 * unitX);
            QPointF p2(midX - 1 * unitY, midY + 1 * unitX);

            QPolygonF polygon;
            polygon << end << p1 << p2;
            drawPolygon(polygon);
        }
    }
};

#endif // PAINTER_H
