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
    ~Mp4Creator();
    void begin(int width, int height);
    void end();
    void addFrame(const QImage &image);
private:
    AVFormatContext *formatCtx;
    const AVCodec *codec;
    AVStream *stream;
    AVCodecContext *codecCtx;
    SwsContext *swsCtx;
    AVFrame *frame;
    AVPacket *packet;
    int64_t pts;
};

#endif // MP4CREATOR_H
