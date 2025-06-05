#include "mp4creator.h"
#include "videowindow.h"

#include <QApplication>
#include <QPainter>
#include <QPen>
#include <functional>

int solve();
void populate_equation();

using ImageCapture = std::function<void(const QImage &)>;

void generateImages(int width, int height, int count, ImageCapture capture)
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

        double p5 = (double) i / 10 + 0.5;
        painter.setPen(pen5);
        painter.drawPoint(320 * cos(p5) + midWidth, 270 * sin(p5) + midHeight);

        double p3 = (double) i / 10 + 0.3;
        painter.setPen(pen4);
        painter.drawPoint(320 * cos(p3) + midWidth, 270 * sin(p3) + midHeight);

        double p1 = (double) i / 10 + 0.1;
        painter.setPen(pen3);
        painter.drawPoint(320 * cos(p1) + midWidth, 270 * sin(p1) + midHeight);

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        capture(image);
    }
}

int main(int argc, char *argv[])
{
    solve();
    populate_equation();

    QApplication app(argc, argv);

    const char *filename = "sample.mp4";
    int width = 1024;
    int height = 768;
    int count = 100;

    Mp4Creator mp4Creator;
    mp4Creator.begin(filename, width, height);
    generateImages(width, height, count, [&mp4Creator](const QImage &image) {
        mp4Creator.addFrame(image);
    });
    mp4Creator.end();

    VideoWindow window(filename);
    window.setWindowTitle("Fiber of Space");
    window.resize(width, height);
    window.show();

    return app.exec();
}
