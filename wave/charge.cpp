#include <QPainter>
#include <QPen>

using ImageCapture = std::function<void(const QImage &)>;

void charge(int width, int height, ImageCapture capture)
{
    int offsetH = width >> 3;
    int offsetV = height >> 3;

    double PI = 4 * atan(1.0);
    double unit = 20;
    double unitH = unit / 2;
    double unitV = unit * cos(PI / 6);

    QPen pen(Qt::black);
    pen.setWidth(3);

    QPainter painter;
    painter.setRenderHint(QPainter::Antialiasing);

    QImage image(width, height, QImage::Format_RGB888);
    image.fill(QColor::fromRgb(255, 255, 255));

    painter.begin(&image);
    painter.setPen(pen);

    for (int h = 0; h < 40; h++) {
        for (int v = 0; v < 30; v++) {
            int shiftH = v % 2 == 0 ? unitH : 0;
            painter.drawPoint(h * unit + offsetH + shiftH, v * unitV + offsetV);
        }
    }

    painter.end();

    image.save("sample.png");
    capture(image);
}
