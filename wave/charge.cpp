#include <QPainter>
#include <QPen>

using ImageCapture = std::function<void(const QImage &)>;

void charge(int width, int height, ImageCapture capture)
{
    int midWidth = width >> 1;
    int midHeight = height >> 1;

    QPen pen(Qt::black);
    pen.setWidth(6);

    QPainter painter;

    QImage image(width, height, QImage::Format_RGB888);
    image.fill(QColor::fromRgb(255, 255, 255));

    painter.begin(&image);

    painter.setPen(pen);
    painter.drawPoint(midWidth, midHeight);

    painter.end();

    capture(image);
}
