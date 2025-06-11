#include <QApplication>
#include "mesh.h"
#include "mp4creator.h"
#include "videowindow.h"

using ImageCapture = std::function<void(const QImage &)>;
void charge(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture);
void spinningSnake(qint32 width, qint32 height, qint32 count, ImageCapture imageCapture);

qint32 main(qint32 argc, char *argv[])
{
    QApplication app(argc, argv);

    const char *filename = "sample.mp4";
    qint32 width = 2 * 1024;
    qint32 height = 2 * 768;

    Mp4Creator mp4Creator;
    auto imageCapture = [&mp4Creator](const QImage &image) { mp4Creator.addFrame(image); };

    mp4Creator.begin(filename, width, height);
    charge(width, height, 3, imageCapture);
    spinningSnake(width, height, 100, imageCapture);
    mp4Creator.end();

    VideoWindow window(filename);
    window.setWindowTitle("Fiber of Space");
    window.resize(width, height);
    window.show();

    return app.exec();
}
