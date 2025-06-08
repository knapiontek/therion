#include "mesh.h"
#include "mp4creator.h"
#include "videowindow.h"

#include <QApplication>

using ImageCapture = std::function<void(const QImage &)>;
void charge(int width, int height, ImageCapture imageCapture);
void spinningSnake(int width, int height, int count, ImageCapture imageCapture);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const char *filename = "sample.mp4";
    int width = 1024;
    int height = 768;
    int count = 10;

    Mp4Creator mp4Creator;
    auto imageCapture = [&mp4Creator](const QImage &image) { mp4Creator.addFrame(image); };

    mp4Creator.begin(filename, width, height);
    charge(width, height, imageCapture);
    spinningSnake(width, height, count, imageCapture);
    mp4Creator.end();

    VideoWindow window(filename);
    window.setWindowTitle("Fiber of Space");
    window.resize(width, height);
    window.show();

    return app.exec();
}
