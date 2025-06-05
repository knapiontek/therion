#include "mp4creator.h"
#include "videowindow.h"

#include <QApplication>

int solve();
void populate_equation();
using ImageCapture = std::function<void(const QImage &)>;
void spinningSnake(int width, int height, int count, ImageCapture capture);

int main(int argc, char *argv[])
{
    solve();
    populate_equation();

    QApplication app(argc, argv);

    const char *filename = "sample.mp4";
    int width = 1024;
    int height = 768;
    int count = 200;

    Mp4Creator mp4Creator;
    mp4Creator.begin(filename, width, height);
    spinningSnake(width, height, count, [&mp4Creator](const QImage &image) {
        mp4Creator.addFrame(image);
    });
    mp4Creator.end();

    VideoWindow window(filename);
    window.setWindowTitle("Fiber of Space");
    window.resize(width, height);
    window.show();

    return app.exec();
}
