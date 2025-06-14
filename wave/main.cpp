#include <QApplication>
#include "charge.h"
#include "mp4creator.h"
#include "spin.h"
#include "torque.h"
#include "videowindow.h"

qint32 main(qint32 argc, char *argv[])
{
    QApplication app(argc, argv);

    const char *filename = "sample.mp4";
    qint32 width = 2 * 1024;
    qint32 height = 2 * 768;

    Mp4Creator mp4Creator;
    auto imageCapture = [&mp4Creator](const QImage &image) { mp4Creator.addFrame(image); };

    mp4Creator.begin(filename, width, height);
    spinningSnake(width, height, 17, imageCapture);
    charge(width, height, 1, imageCapture);
    torque(width, height, 1, imageCapture);
    mp4Creator.end();

    VideoWindow window(filename);
    window.setWindowTitle("Fiber of Space");
    window.resize(width, height);
    window.show();

    return app.exec();
}
