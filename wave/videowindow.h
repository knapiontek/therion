#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPainter>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

class VideoWindow : public QMainWindow
{
public:
    VideoWindow(const char *filename)
        : QMainWindow()
    {
        videoWidget = new QVideoWidget(this);
        player = new QMediaPlayer(this);

        player->setVideoOutput(videoWidget);
        player->setSource(QUrl::fromLocalFile(filename));
        player->play();

        setCentralWidget(videoWidget);
    }

    ~VideoWindow() override { stopMedia(); }

public slots:
    void stopMedia()
    {
        if (player) {
            if (player->playbackState() == QMediaPlayer::PlayingState) {
                player->stop();
            }
            player->setSource(QUrl());
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

#endif // VIDEOWINDOW_H
