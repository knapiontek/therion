#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include "common.h"
#include "solve.h"

const qreal unit = 1;
const qreal unitH = unit / 2;
const qreal unitV = unit * sqrt(3) / 2;

InputMesh buildMesh(qint32 sizeH, qint32 sizeV)
{
    InputMesh input;

    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    // points
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH; h++) {
            qreal shiftH = v % 2 == 0 ? unitH : 0;
            input.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    // fixes on horizontal borders
    for (qint32 i = 0; i < sizeH; i++) {
        input.fixMap.insert(index(i, 0), Fix2D{true, true});
        input.fixMap.insert(index(i, sizeV - 1), Fix2D{true, true});
    }

    // fixes on vertical borders
    for (qint32 i = 0; i < sizeV; i++) {
        input.fixMap.insert(index(0, i), Fix2D{true, true});
        input.fixMap.insert(index(sizeH - 1, i), Fix2D{true, true});
    }

    // horizontal elements
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH - 1; h++) {
            input.elementSeq.append(Element{index(h, v), index(h + 1, v)});
        }
    }

    // vertical elements
    for (qint32 h = 0; h < sizeH; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            input.elementSeq.append(Element{index(h, v), index(h, v + 1)});
        }
    }

    // diagonal elements
    for (qint32 h = 0; h < sizeH - 1; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                input.elementSeq.append(Element{index(h, v), index(h + 1, v + 1)});
            } else {
                input.elementSeq.append(Element{index(h + 1, v), index(h, v + 1)});
            }
        }
    }

    return input;
}

void applyForce(InputMesh &input, qint32 point, qint32 length, qint32 sizeH, qint32 sizeV)
{
    struct
    {
        qint32 i;
        Point2D p;
    } vectorSeq[]{{point - 1, Point2D{-unit * length, 0}},
                  {point + 1, Point2D{unit * length, 0}},
                  {point - sizeH + 1, Point2D{unitH * length, -unitV * length}},
                  {point - sizeH, Point2D{-unitH * length, -unitV * length}},
                  {point + sizeH + 1, Point2D{unitH * length, unitV * length}},
                  {point + sizeH, Point2D{-unitH * length, unitV * length}}};
    for (auto &v : vectorSeq) {
        input.forceMap.insert(v.i, v.p);
    }
}

struct Painter : public QPainter
{
    void drawArrow(const QPointF &begin, const QPointF &end)
    {
        qreal x = end.x() - begin.x();
        qreal y = end.y() - begin.y();

        qreal length = sqrt(x * x + y * y);

        qreal unitX = x / length;
        qreal unitY = y / length;

        qreal midX = end.x() - 12 * unitX;
        qreal midY = end.y() - 12 * unitY;

        QPointF p1(midX + 3 * unitY, midY - 3 * unitX);
        QPointF p2(midX - 3 * unitY, midY + 3 * unitX);

        drawLine(begin, end);
        QPolygonF polygon;
        polygon << end << p1 << p2;
        drawPolygon(polygon);
    }
};

void charge(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    qint32 sizeH = 1 * 42;
    qint32 sizeV = 1 * 36;
    qreal unit = width / sizeH;

    auto scale = [&unit](Point2D &p) { return QPointF(unit * p.x + 24, unit * p.y + 24); };
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    InputMesh input = buildMesh(sizeH, sizeV);

    for (qint32 i = 0; i < count; i++) {
        applyForce(input,
                   index(sizeH / 3, sizeV * 2 / 3),
                   3000 + 10000 * cos((qreal) i / 10),
                   sizeH,
                   sizeV);
        applyForce(input,
                   index(sizeH * 2 / 3, sizeV / 3),
                   3000 - 10000 * sin((qreal) i / 10),
                   sizeH,
                   sizeV);

        OutputMesh output = solveMesh(input);

        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        QPen fixPen(Qt::red);
        fixPen.setWidth(3);

        QPen forcePen(Qt::yellow);
        forcePen.setWidth(4);

        QPen meshPen(Qt::white);
        meshPen.setWidthF(0.5);

        Painter painter;
        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(Qt::white);

        // elements
        painter.setPen(meshPen);
        for (auto &e : input.elementSeq) {
            auto &p1 = output.pointSeq[e.p1];
            auto &p2 = output.pointSeq[e.p2];
            painter.drawArrow(scale(p1), scale(p2));
        }

        // fixes
        painter.setPen(fixPen);
        for (auto it = input.fixMap.begin(); it != input.fixMap.end(); ++it) {
            auto &p = output.pointSeq[it.key()];
            painter.drawPoint(scale(p));
        }

        // forces
        painter.setPen(forcePen);
        for (auto it = input.forceMap.begin(); it != input.forceMap.end(); ++it) {
            auto &p = output.pointSeq[it.key()];
            painter.drawPoint(scale(p));
        }

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        qDebug() << "charge frame: " << i;
        image.save("sample.png");
        imageCapture(image);
    }
}
