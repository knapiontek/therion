#include "mainwindow.h"
#include "mp4creator.h"

#include <QApplication>
#include <QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int width = 1024;
    int height = 768;
    int count = 2000;

    Mp4Creator mp4Creator;
    mp4Creator.init(width, height);

    QPen pen(Qt::white);
    pen.setWidth(5);

    for (int i = 0; i < count; ++i) {
        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromHsv(255, 255, 255));

        QPainter painter(&image);
        painter.setPen(pen);
        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i+1));
        painter.drawPoint(220 * cos((double)i/10) + 512, 170 * sin((double)i/10) + 384);
        painter.end();

        mp4Creator.addFrame(image);
    }

    mp4Creator.destroy();

    MainWindow w;
    w.show();

    return a.exec();
}
