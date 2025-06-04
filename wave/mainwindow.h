#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QPainter>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

class MainWindow : public QMainWindow
{
public:
    MainWindow(const char *filename)
        : QMainWindow()
    {
        videoWidget = new QVideoWidget(this);
        player = new QMediaPlayer(this);

        player->setVideoOutput(videoWidget);
        player->setSource(QUrl::fromLocalFile(filename));
        player->play();

        setCentralWidget(videoWidget);
    }

    ~MainWindow() override { stopMedia(); }

public slots:
    void stopMedia()
    {
        if (player) {
            if (player->playbackState() == QMediaPlayer::PlayingState) {
                player->stop();
            }
            player->setSource(QUrl()); // Clear media
            player->deleteLater();
        }
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        stopMedia();
        QMainWindow::closeEvent(event);
    }

private:
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
};

#endif // MAINWINDOW_H
