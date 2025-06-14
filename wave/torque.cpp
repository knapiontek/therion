#include "torque.h"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include "common.h"
#include "painter.h"
#include "solve.h"

void torque(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    qint32 sizeH = 1 * 42;
    qint32 sizeV = 1 * 36;
    qreal unit = width / sizeH;

    auto scale = [&unit](const Point2D &p) { return QPointF(unit * p.x + 24, unit * p.y + 24); };
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    qint32 force1 = index(sizeH / 2 - 4, sizeV / 2);
    qint32 force2 = index(sizeH / 2 + 4, sizeV / 2);

    InputMesh inputMesh = buildMesh(sizeH, sizeV);

    for (qint32 i = 0; i < count; i++) {
        qint32 length = EA * cos((qreal) i / 10);
        inputMesh.forceMap.insert(force1, Point2D{0, -1} * length);
        inputMesh.forceMap.insert(force2, Point2D{0, 1} * length);

        OutputMesh outputMesh = solveMesh(inputMesh);

        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        Painter painter;
        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // points
        QPen deltaPen(Qt::white);
        deltaPen.setWidth(1);
        painter.setPen(deltaPen);
        for (qint32 i = 0; i < inputMesh.pointSeq.size(); ++i) {
            auto &p = inputMesh.pointSeq[i];
            auto &d = outputMesh.deltaSeq[i];
            painter.drawArrow(scale(p), scale(p + d * 2));
        }

        // forces
        QPen forcePen(Qt::yellow);
        forcePen.setWidth(1);
        painter.setPen(forcePen);
        painter.setBrush(Qt::yellow);
        for (auto it = inputMesh.forceMap.begin(); it != inputMesh.forceMap.end(); ++it) {
            auto &p = inputMesh.pointSeq[it.key()];
            auto &d = outputMesh.deltaSeq[it.key()];
            auto &f = it.value();
            qDebug() << "torque delta" << d << "force" << f << "at point" << p;
            if (f * f > 0.07 * EA * EA)
                painter.drawArrow(scale(p), scale(p + f * 2 / EA));
        }

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("torque frame %1").arg(i + 1));

        painter.end();

        qDebug() << "torque frame:" << i;
        image.save("torque.png");
        imageCapture(image);
    }
}
