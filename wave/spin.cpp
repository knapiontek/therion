#include <QDebug>
#include <QPainter>
#include <QPen>
#include "common.h"

void spinningSnake(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture)
{
    auto scale = [&width, &height](qreal angle) {
        return QPointF(320 * cos(angle) + (width >> 1), 270 * sin(angle) + (height >> 1));
    };

    QPen pen5(Qt::white);
    pen5.setWidth(5);
    QPen pen4(Qt::white);
    pen4.setWidth(4);
    QPen pen3(Qt::white);
    pen3.setWidth(3);

    QPainter painter;

    for (qint32 i = 0; i < count; ++i) {
        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(0, 0, 0));

        painter.begin(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.setPen(pen5);
        painter.drawPoint(scale((qreal) i / 10 + 0.10));
        painter.setPen(pen4);
        painter.drawPoint(scale((qreal) i / 10 + 0.05));
        painter.setPen(pen3);
        painter.drawPoint(scale((qreal) i / 10 + 0.00));

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        qDebug() << "snake frame: " << i;
        imageCapture(image);
    }
}
