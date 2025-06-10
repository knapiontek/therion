#include <QDebug>
#include <QPainter>
#include <QPen>
#include "mesh.h"

void solveMesh(MeshInput &input, MeshOutput &output);

const double unit = 1;
const double unitH = unit / 2;
const double unitV = unit * sqrt(3) / 2;

void buildMesh(MeshInput &mesh, int sizeH, int sizeV)
{
    auto index = [&sizeH](int h, int v) { return h + (v * sizeH); };

    // points
    for (int v = 0; v < sizeV; v++) {
        for (int h = 0; h < sizeH; h++) {
            double shiftH = v % 2 == 0 ? unitH : 0;
            mesh.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    // fixes on horizontal borders
    for (int i = 0; i < sizeH; i++) {
        mesh.fixMap.insert(index(i, 0), Fix2D{true, true});
        mesh.fixMap.insert(index(i, sizeV - 1), Fix2D{true, true});
    }

    // fixes on vertical borders
    for (int i = 0; i < sizeV; i++) {
        mesh.fixMap.insert(index(0, i), Fix2D{true, true});
        mesh.fixMap.insert(index(sizeH - 1, i), Fix2D{true, true});
    }

    // horizontal elements
    for (int v = 0; v < sizeV; v++) {
        for (int h = 0; h < sizeH - 1; h++) {
            mesh.elementSeq.append(Element{index(h, v), index(h + 1, v)});
        }
    }

    // vertical elements
    for (int h = 0; h < sizeH; h++) {
        for (int v = 0; v < sizeV - 1; v++) {
            mesh.elementSeq.append(Element{index(h, v), index(h, v + 1)});
        }
    }

    // diagonal elements
    for (int h = 0; h < sizeH - 1; h++) {
        for (int v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                mesh.elementSeq.append(Element{index(h, v), index(h + 1, v + 1)});
            } else {
                mesh.elementSeq.append(Element{index(h, v + 1), index(h + 1, v)});
            }
        }
    }
}

void applyForce(MeshInput &mesh, int point, int length, int sizeH, int sizeV)
{
    struct
    {
        int i;
        Point2D p;
    } vectorSeq[]{{point - 1, Point2D{-unit * length, 0}},
                  {point + 1, Point2D{unit * length, 0}},
                  {point - sizeH, Point2D{unitH * length, -unitV * length}},
                  {point - sizeH - 1, Point2D{-unitH * length, -unitV * length}},
                  {point + sizeH, Point2D{unitH * length, unitV * length}},
                  {point + sizeH - 1, Point2D{-unitH * length, unitV * length}}};
    for (auto &v : vectorSeq) {
        mesh.forceMap.insert(v.i, v.p);
    }
}

using ImageCapture = std::function<void(const QImage &)>;

void charge(int width, int height, int count, ImageCapture imageCapture)
{
    int sizeH = 40;
    int sizeV = 35;
    double unit = width / sizeH;

    auto scale = [&unit](Point2D &p) { return QPointF(unit * p.x + 10, unit * p.y + 10); };
    auto index = [&sizeH](int h, int v) { return h + (v * sizeH); };

    MeshInput meshInput;
    MeshOutput meshOutput;
    buildMesh(meshInput, sizeH, sizeV);

    for (int i = 0; i < count; i++) {
        applyForce(meshInput,
                   index(sizeH / 3, sizeV * 2 / 3),
                   1000 + 10000 * cos((double) i / 10),
                   sizeH,
                   sizeV);
        applyForce(meshInput,
                   index(sizeH * 2 / 3, sizeV / 3),
                   1000 + -10000 * sin((double) i / 10),
                   sizeH,
                   sizeV);
        solveMesh(meshInput, meshOutput);

        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        QPen redPen(Qt::red);
        redPen.setWidth(3);

        QPen greenPen(Qt::green);
        greenPen.setWidth(3);

        QPen whitePen(Qt::white);
        whitePen.setWidthF(0.5);

        QPainter painter;
        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // elements
        painter.setPen(whitePen);
        for (auto &e : meshInput.elementSeq) {
            auto &p1 = meshOutput.pointSeq[e.p1];
            auto &p2 = meshOutput.pointSeq[e.p2];
            painter.drawLine(scale(p1), scale(p2));
        }

        // fixes
        painter.setPen(redPen);
        for (auto it = meshInput.fixMap.begin(); it != meshInput.fixMap.end(); ++it) {
            auto &p = meshOutput.pointSeq[it.key()];
            painter.drawPoint(scale(p));
        }

        // forces
        painter.setPen(greenPen);
        for (auto it = meshInput.forceMap.begin(); it != meshInput.forceMap.end(); ++it) {
            auto &p = meshOutput.pointSeq[it.key()];
            painter.drawPoint(scale(p));
        }

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        qDebug() << "charge frame: " << i;
        imageCapture(image);
    }
}
