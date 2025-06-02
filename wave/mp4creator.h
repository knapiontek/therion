#ifndef MP4CREATOR_H
#define MP4CREATOR_H

#include <QImage>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class Mp4Creator
{
public:
    Mp4Creator();
    void init(int width, int height);
    void addFrame(const QImage &image);
private:
    void init();
    void destroy();
private:
    AVFormatContext *formatCtx;
    AVCodecContext *codecCtx;
    const AVCodec *codec;
    AVStream *stream;
    SwsContext *swsCtx;
    AVFrame *frame;
    AVPacket *packet;
    int64_t pts;
};

#endif // MP4CREATOR_H
