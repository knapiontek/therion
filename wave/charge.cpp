#include <QPainter>
#include <QPen>
#include "mesh.h"

void solve(MeshInput &input, MeshOutput &output);

void buildMesh(MeshInput &mesh, int sizeH, int sizeV)
{
    double unit = 1;
    double unitH = unit / 2;
    double unitV = unit * sqrt(3) / 2;

    for (int v = 0; v < sizeV; v++) {
        for (int h = 0; h < sizeH; h++) {
            double shiftH = v % 2 == 0 ? unitH : 0;
            mesh.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    for (int i = 0; i < sizeV - 1; i++) {
        mesh.fixMap.insert(i, Fix2D{true, true});
        mesh.fixMap.insert((i + 1) * sizeH, Fix2D{true, true});
    }

    for (int v = 0; v < sizeV; v++) {
        for (int h = 0; h < sizeH - 1; h++) {
            int p1 = v * sizeH + h;
            int p2 = v * sizeH + h + 1;
            mesh.elementSeq.append(Element{p1, p2});
        }
    }

    for (int h = 0; h < sizeH; h++) {
        for (int v = 0; v < sizeV - 1; v++) {
            int p1 = v * sizeH + h;
            int p2 = (v + 1) * sizeH + h;
            mesh.elementSeq.append(Element{p1, p2});
        }
    }

    for (int h = 0; h < sizeH - 1; h++) {
        for (int v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                int p1 = v * sizeH + h;
                int p2 = (v + 1) * sizeH + h + 1;
                mesh.elementSeq.append(Element{p1, p2});
            } else {
                int p1 = (v + 1) * sizeH + h;
                int p2 = v * sizeH + h + 1;
                mesh.elementSeq.append(Element{p1, p2});
            }
        }
    }

    int lastPoint = (sizeH - 1) * (sizeV - 1);
    mesh.forceMap.insert(lastPoint, Point2D{20, -20});
}

using ImageCapture = std::function<void(const QImage &)>;

void charge(int width, int height, ImageCapture imageCapture)
{
    QImage image(width, height, QImage::Format_RGB888);
    image.fill(QColor::fromRgb(255, 255, 255));

    QPen redPen(Qt::red);
    redPen.setWidth(3);

    QPen greenPen(Qt::green);
    greenPen.setWidth(2);

    QPen blackPen(Qt::black);
    blackPen.setWidthF(0.5);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int sizeH = 40;
    int sizeV = 35;
    double unit = width / sizeH;
    auto scale = [&unit](Point2D &p) { return QPointF(unit * p.x + 18, unit * p.y + 18); };

    MeshInput meshInput;
    MeshOutput meshOutput;
    buildMesh(meshInput, sizeH, sizeV);
    solve(meshInput, meshOutput);

    painter.setPen(blackPen);
    for (auto &e : meshInput.elementSeq) {
        auto &p1 = meshInput.pointSeq[e.p1];
        auto &p2 = meshInput.pointSeq[e.p2];
        painter.drawLine(scale(p1), scale(p2));
    }

    painter.setPen(greenPen);
    for (auto &p : meshInput.pointSeq) {
        painter.drawPoint(scale(p));
    }

    painter.setPen(redPen);
    for (auto it = meshInput.fixMap.begin(); it != meshInput.fixMap.end(); ++it) {
        auto &p = meshInput.pointSeq[it.key()];
        painter.drawPoint(scale(p));
    }

    painter.end();

    image.save("sample.png");
    imageCapture(image);
}
