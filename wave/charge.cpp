#include <QDebug>
#include <QPainter>
#include <QPen>
#include "mesh.h"

void solveMesh(MeshInput &input, MeshOutput &output);

const qreal unit = 1;
const qreal unitH = unit / 2;
const qreal unitV = unit * sqrt(3) / 2;

void buildMesh(MeshInput &mesh, qint32 sizeH, qint32 sizeV)
{
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    // points
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH; h++) {
            qreal shiftH = v % 2 == 0 ? unitH : 0;
            mesh.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    // fixes on horizontal borders
    for (qint32 i = 0; i < sizeH; i++) {
        mesh.fixMap.insert(index(i, 0), Fix2D{true, true});
        mesh.fixMap.insert(index(i, sizeV - 1), Fix2D{true, true});
    }

    // fixes on vertical borders
    for (qint32 i = 0; i < sizeV; i++) {
        mesh.fixMap.insert(index(0, i), Fix2D{true, true});
        mesh.fixMap.insert(index(sizeH - 1, i), Fix2D{true, true});
    }

    // horizontal elements
    for (qint32 v = 0; v < sizeV; v++) {
        for (qint32 h = 0; h < sizeH - 1; h++) {
            mesh.elementSeq.append(Element{index(h, v), index(h + 1, v)});
        }
    }

    // vertical elements
    for (qint32 h = 0; h < sizeH; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            mesh.elementSeq.append(Element{index(h, v), index(h, v + 1)});
        }
    }

    // diagonal elements
    for (qint32 h = 0; h < sizeH - 1; h++) {
        for (qint32 v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                mesh.elementSeq.append(Element{index(h, v), index(h + 1, v + 1)});
            } else {
                mesh.elementSeq.append(Element{index(h, v + 1), index(h + 1, v)});
            }
        }
    }
}

void applyForce(MeshInput &mesh, qint32 point, qint32 length, qint32 sizeH, qint32 sizeV)
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
        mesh.forceMap.insert(v.i, v.p);
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
        drawLine(begin, end);
    }
};

using ImageCapture = std::function<void(const QImage &)>;

void charge(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    qint32 sizeH = 1 * 42;
    qint32 sizeV = 1 * 36;
    qreal unit = width / sizeH;

    auto scale = [&unit](Point2D &p) { return QPointF(unit * p.x + 20, unit * p.y + 20); };
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    MeshInput meshInput;
    MeshOutput meshOutput;
    buildMesh(meshInput, sizeH, sizeV);

    for (qint32 i = 0; i < count; i++) {
        applyForce(meshInput,
                   index(sizeH / 3, sizeV * 2 / 3),
                   2000 + 10000 * cos((qreal) i / 10),
                   sizeH,
                   sizeV);
        applyForce(meshInput,
                   index(sizeH * 2 / 3, sizeV / 3),
                   2000 - 10000 * sin((qreal) i / 10),
                   sizeH,
                   sizeV);
        solveMesh(meshInput, meshOutput);

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

        // elements
        painter.setPen(meshPen);
        for (auto &e : meshInput.elementSeq) {
            auto &p1 = meshOutput.pointSeq[e.p1];
            auto &p2 = meshOutput.pointSeq[e.p2];
            painter.drawArrow(scale(p1), scale(p2));
        }

        // fixes
        painter.setPen(fixPen);
        for (auto it = meshInput.fixMap.begin(); it != meshInput.fixMap.end(); ++it) {
            auto &p = meshOutput.pointSeq[it.key()];
            painter.drawPoint(scale(p));
        }

        // forces
        painter.setPen(forcePen);
        for (auto it = meshInput.forceMap.begin(); it != meshInput.forceMap.end(); ++it) {
            auto &p = meshOutput.pointSeq[it.key()];
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
