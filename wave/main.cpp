#include "mp4creator.h"

#include <QApplication>
#include <QMediaPlayer>
#include <QPainter>
#include <QVideoWidget>

int solve();
void populate_equation();

int main(int argc, char *argv[])
{
    solve();
    populate_equation();

    QApplication a(argc, argv);

    int width = 1024;
    int height = 768;
    int count = 100;

    Mp4Creator mp4Creator;
    mp4Creator.begin(width, height);

    QPen pen5(Qt::white);
    pen5.setWidth(5);
    QPen pen4(Qt::white);
    pen4.setWidth(4);
    QPen pen3(Qt::white);
    pen3.setWidth(3);

    QPainter painter;

    for (int i = 0; i < count; ++i) {
        QImage image(width, height, QImage::Format_RGB888);
        image.fill(QColor::fromHsv(255, 255, 255));

        painter.begin(&image);

        double p5 = (double) i / 10 + 0.5;
        painter.setPen(pen5);
        painter.drawPoint(320 * cos(p5) + 512, 270 * sin(p5) + 384);

        double p3 = (double) i / 10 + 0.3;
        painter.setPen(pen4);
        painter.drawPoint(320 * cos(p3) + 512, 270 * sin(p3) + 384);

        double p1 = (double) i / 10 + 0.1;
        painter.setPen(pen3);
        painter.drawPoint(320 * cos(p1) + 512, 270 * sin(p1) + 384);

        painter.setFont(QFont("Arial", 20));
        painter.drawText(image.rect(), Qt::AlignTop, QString("Frame %1").arg(i + 1));

        painter.end();

        mp4Creator.addFrame(image);
    }

    mp4Creator.end();

    QMediaPlayer *player = new QMediaPlayer;
    QVideoWidget *videoWidget = new QVideoWidget;

    player->setVideoOutput(videoWidget);

    player->setSource(QUrl::fromLocalFile("output.mp4"));

    videoWidget->setWindowTitle("Fiber of Space");
    videoWidget->resize(width, height);
    videoWidget->show();

    player->play();

    return a.exec();
}
