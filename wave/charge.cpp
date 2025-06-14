#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include "common.h"
#include "painter.h"
#include "solve.h"

void charge(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    qint32 sizeH = 1 * 42;
    qint32 sizeV = 1 * 36;
    qreal unit = width / sizeH;

    auto scale = [&unit](const Point2D &p) { return QPointF(unit * p.x + 24, unit * p.y + 24); };
    auto index = [&sizeH](qint32 h, qint32 v) { return h + (v * sizeH); };

    qint32 force1 = index(sizeH / 3, sizeV * 2 / 3);
    qint32 force2 = index(sizeH * 2 / 3, sizeV / 3);

    InputMesh inputMesh = buildMesh(sizeH, sizeV);

    for (qint32 i = 0; i < count; i++) {
        applyStarForce(inputMesh, force1, EA * cos((qreal) i / 10), sizeH, sizeV);
        applyStarForce(inputMesh, force2, -EA * cos((qreal) i / 10), sizeH, sizeV);

        OutputMesh outputMesh = solveMesh(inputMesh);

        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        Painter painter;
        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // points
        for (qint32 i = 0; i < inputMesh.pointSeq.size(); ++i) {
            auto &p = inputMesh.pointSeq[i];
            auto delta = outputMesh.deltaSeq[i];
            int color = std::fmin(0xFF * 30 * (delta * delta), 0xFF);
            QPen pen(QColor::fromRgb(color, color, color));
            pen.setWidth(4);
            painter.setPen(pen);
            painter.drawPoint(scale(p));
        }

        // forces
        QPen forcePen(Qt::yellow);
        forcePen.setWidth(1);
        painter.setPen(forcePen);
        painter.setBrush(Qt::yellow);
        for (auto it = inputMesh.forceMap.begin(); it != inputMesh.forceMap.end(); ++it) {
            auto &p = inputMesh.pointSeq[it.key()];
            auto &f = it.value();
            if (f * f > 0.07 * EA * EA)
                painter.drawArrow(scale(p), scale(p + f * .8 / EA));
        }

        if (false) {
            // elements
            QPen meshPen(Qt::red);
            meshPen.setWidth(3);
            painter.setPen(meshPen);
            for (auto &e : inputMesh.elementSeq) {
                auto &p1 = inputMesh.pointSeq[e.p1];
                auto &p2 = inputMesh.pointSeq[e.p2];
                auto &dp1 = outputMesh.deltaSeq[e.p1];
                auto &dp2 = outputMesh.deltaSeq[e.p2];
                painter.drawArrow(scale(p1 + dp1), scale(p2 + dp2));
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

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("charge frame %1").arg(i + 1));

        painter.end();

        qDebug() << "charge frame: " << i;
        image.save("charge.png");
        imageCapture(image);
    }
}
