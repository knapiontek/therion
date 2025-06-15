#include "torque.h"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include "common.h"
#include "painter.h"
#include "solve.h"

void torque(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    qint32 sizeH = 2 * 42;
    qint32 sizeV = 2 * 36;
    qreal unit = width / sizeH;

    auto scale = [&unit](const Point2D &p) { return QPointF(unit * p.x + 24, unit * p.y + 24); };
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    qint32 force1 = index(sizeH / 2 - 4, sizeV / 2);
    qint32 force2 = index(sizeH / 2 + 4, sizeV / 2);

    InputMesh inputMesh = buildMesh(sizeH, sizeV);

    for (qint32 i = 0; i < count; i++) {
        qreal length1 = EA * cos(1.0 * i / 10);
        qreal length2 = EA * cos(1.2 * i / 10);
        inputMesh.forceMap.insert(force1, Point2D{0, -1} * length1);
        inputMesh.forceMap.insert(force2, Point2D{0, 1} * length2);

        OutputMesh outputMesh = solveMesh(inputMesh);

        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        Painter painter;
        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // points
        for (qint32 i = 0; i < inputMesh.pointSeq.size(); ++i) {
            auto &point = inputMesh.pointSeq[i];
            auto &delta = outputMesh.deltaSeq[i];
            int color = std::fmin(0xFF * 30 * (delta * delta), 0xFF);
            QPen pen(QColor::fromRgb(color, color, color));
            pen.setWidth(1);
            painter.setPen(pen);
            painter.drawArrow(scale(point), scale(point + delta * 2));
        }

        // forces
        QPen forcePen(Qt::yellow);
        forcePen.setWidth(1);
        painter.setPen(forcePen);
        painter.setBrush(Qt::yellow);
        for (auto it = inputMesh.forceMap.begin(); it != inputMesh.forceMap.end(); ++it) {
            auto &point = inputMesh.pointSeq[it.key()];
            auto &delta = outputMesh.deltaSeq[it.key()];
            auto &force = it.value();
            qDebug() << "torque delta" << delta << "force" << force << "at point" << point;
            painter.drawArrow(scale(point), scale(point + force * 2 / EA));
        }

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("torque frame %1").arg(i + 1));

        painter.end();

        qDebug() << "torque frame:" << i;
        image.save("torque.png");
        imageCapture(image);
    }
}
