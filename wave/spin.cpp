#include <QPainter>
#include <QPen>

using ImageCapture = std::function<void(const QImage &)>;

void spinningSnake(int width, int height, int count, ImageCapture imageCapture)
{
    auto scale = [&width, &height](double angle) {
        return QPointF(320 * cos(angle) + (width >> 1), 270 * sin(angle) + (height >> 1));
    };

    QPen pen5(Qt::black);
    pen5.setWidth(5);
    QPen pen4(Qt::black);
    pen4.setWidth(4);
    QPen pen3(Qt::black);
    pen3.setWidth(3);

    QPainter painter;

    for (int i = 0; i < count; ++i) {
        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromRgb(255, 255, 255));

        painter.begin(&image);

        painter.setPen(pen5);
        painter.drawPoint(scale((double) i / 10 + 0.20));
        painter.setPen(pen4);
        painter.drawPoint(scale((double) i / 10 + 0.15));
        painter.setPen(pen3);
        painter.drawPoint(scale((double) i / 10 + 0.10));

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        imageCapture(image);
    }
}
