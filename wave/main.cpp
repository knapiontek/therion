#include "mainwindow.h"
#include "mp4creator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Mp4Creator mp4Creator;

    MainWindow w;
    w.show();

    return a.exec();
}
