#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include "common.h"
#include "painter.h"
#include "solve.h"

const qreal unit = 1;
const qreal unitH = unit / 2;
const qreal unitV = unit * std::sqrt(3) / 2;

InputMesh buildMesh(qint32 sizeH, qint32 sizeV)
{
    InputMesh inputMesh;

    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    // points
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH; h++) {
            qreal shiftH = v % 2 == 0 ? unitH : 0;
            inputMesh.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    // fixes on horizontal borders
    for (qint32 i = 0; i < sizeH; i++) {
        inputMesh.fixMap.insert(index(i, 0), Fix2D{true, true});
        inputMesh.fixMap.insert(index(i, sizeV - 1), Fix2D{true, true});
    }

    // fixes on vertical borders
    for (qint32 i = 0; i < sizeV; i++) {
        inputMesh.fixMap.insert(index(0, i), Fix2D{true, true});
        inputMesh.fixMap.insert(index(sizeH - 1, i), Fix2D{true, true});
    }

    // horizontal elements
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH - 1; h++) {
            inputMesh.elementSeq.append(Element{index(h, v), index(h + 1, v)});
        }
    }

    // vertical elements
    for (qint32 h = 0; h < sizeH; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            inputMesh.elementSeq.append(Element{index(h, v), index(h, v + 1)});
        }
    }

    // diagonal elements
    for (qint32 h = 0; h < sizeH - 1; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                inputMesh.elementSeq.append(Element{index(h, v), index(h + 1, v + 1)});
            } else {
                inputMesh.elementSeq.append(Element{index(h + 1, v), index(h, v + 1)});
            }
        }
    }

    return inputMesh;
}

void applyForce(InputMesh &inputMesh, qint32 point, qint32 length, qint32 sizeH, qint32 sizeV)
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
        inputMesh.forceMap.insert(v.i, v.p);
    }
}

void charge(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    qreal EA = 10000; // Young * Area
    qint32 sizeH = 1 * 42;
    qint32 sizeV = 1 * 36;
    qreal unit = width / sizeH;

    auto scale = [&unit](const Point2D &p) { return QPointF(unit * p.x + 24, unit * p.y + 24); };
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    qint32 force1 = index(sizeH / 3, sizeV * 2 / 3);
    qint32 force2 = index(sizeH * 2 / 3, sizeV / 3);

    InputMesh inputMesh = buildMesh(sizeH, sizeV);

    for (qint32 i = 0; i < count; i++) {
        applyForce(inputMesh, force1, EA * cos((qreal) i / 10), sizeH, sizeV);
        applyForce(inputMesh, force2, -EA * cos((qreal) i / 10), sizeH, sizeV);

        OutputMesh outputMesh = solveMesh(inputMesh);

        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        Painter painter;
        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // points
        for (qint32 i = 0; i < inputMesh.pointSeq.size(); ++i) {
            auto &p = inputMesh.pointSeq[i];
            auto d = outputMesh.deltaSeq[i];
            int c = std::fmin(0xFF * 3 * d, 0xFF); // 0.4 is max force
            QPen pen(QColor::fromRgb(c, c, c));
            pen.setWidth(4);
            painter.setPen(pen);
            painter.drawPoint(scale(p));
        }

        if (false) {
            // elements
            QPen meshPen(Qt::red);
            meshPen.setWidth(3);
            painter.setPen(meshPen);
            for (auto &e : inputMesh.elementSeq) {
                auto &p1 = outputMesh.pointSeq[e.p1];
                auto &p2 = outputMesh.pointSeq[e.p2];
                painter.drawArrow(scale(p1), scale(p2));
            }

            // fixes
            QPen fixPen(Qt::red);
            fixPen.setWidth(3);
            painter.setPen(fixPen);
            for (auto it = inputMesh.fixMap.begin(); it != inputMesh.fixMap.end(); ++it) {
                auto &p = inputMesh.pointSeq[it.key()];
                painter.drawPoint(scale(p));
            }
        }

        // forces
        QPen forcePen(Qt::yellow);
        forcePen.setWidth(1);
        painter.setPen(forcePen);
        painter.setBrush(Qt::yellow);
        for (auto it = inputMesh.forceMap.begin(); it != inputMesh.forceMap.end(); ++it) {
            auto &p = inputMesh.pointSeq[it.key()];
            painter.drawPoint(scale(p));

            auto &f = it.value();
            if (f.x * f.x + f.y * f.y > 0.1 * EA * EA) {
                Point2D end{p.x + f.x / EA, p.y + f.y / EA};
                painter.drawArrow(scale(p), scale(end));
            }
        }

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        qDebug() << "charge frame: " << i;
        image.save("sample.png");
        imageCapture(image);
    }
}
