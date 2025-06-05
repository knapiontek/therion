#include <QPainter>
#include <QPen>

using ImageCapture = std::function<void(const QImage &)>;

void spinningSnake(int width, int height, int count, ImageCapture capture)
{
    int midWidth = width >> 1;
    int midHeight = height >> 1;

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

        double p5 = (double) i / 10 + 0.20;
        painter.setPen(pen5);
        painter.drawPoint(320 * cos(p5) + midWidth, 270 * sin(p5) + midHeight);

        double p3 = (double) i / 10 + 0.15;
        painter.setPen(pen4);
        painter.drawPoint(320 * cos(p3) + midWidth, 270 * sin(p3) + midHeight);

        double p1 = (double) i / 10 + 0.10;
        painter.setPen(pen3);
        painter.drawPoint(320 * cos(p1) + midWidth, 270 * sin(p1) + midHeight);

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        capture(image);
    }
}
