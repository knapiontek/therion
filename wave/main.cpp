#include "mainwindow.h"
#include "mp4creator.h"

#include <QApplication>
#include <QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int width = 640;
    int height = 480;
    int count = 10;

    Mp4Creator mp4Creator;
    mp4Creator.init(width, height);

    for (int i = 0; i < count; ++i) {
        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromHsv((i * 36) % 360, 255, 255)); // Cycle hues

        QPainter painter(&image);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignCenter, QString("Borys The Lion %1").arg(i));
        painter.end();

        mp4Creator.addFrame(image);
    }

    mp4Creator.destroy();

    MainWindow w;
    w.show();

    return a.exec();
}
