#include <stdexcept>

#include "mp4creator.h"

Mp4Creator::Mp4Creator()
    : formatCtx(nullptr)
    , codecCtx(nullptr)
    , codec(nullptr)
    , stream(nullptr)
    , swsCtx(nullptr)
    , frame(nullptr)
    , packet(nullptr)
    , pts(0)
{

}

void Mp4Creator::init(int width, int height)
{
    const char *filename = "output.mp4";

    avformat_network_init();

    avformat_alloc_output_context2(&formatCtx, nullptr, "mp4", filename);
    if (!formatCtx) {
        throw std::runtime_error("fail to create format context");
    }

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        throw std::runtime_error("fail to find encoder H.264");
    }

    stream = avformat_new_stream(formatCtx, nullptr);
    if (!stream) {
        throw std::runtime_error("fail to create stream");
    }

    codecCtx = avcodec_alloc_context3(codec);
    codecCtx->codec_id = AV_CODEC_ID_H264;
    codecCtx->width = width;
    codecCtx->height = height;
    codecCtx->time_base = {1, 30};
    codecCtx->gop_size = 10;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    codecCtx->bit_rate = 400000;

    if (formatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        throw std::runtime_error("fail to open codec");
    }

    avcodec_parameters_from_context(stream->codecpar, codecCtx);

    if (!(formatCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&formatCtx->pb, filename, AVIO_FLAG_WRITE) < 0) {
            throw std::runtime_error("fail to open output file");
        }
    }

    int ret = avformat_write_header(formatCtx, nullptr);
    if (ret < 0) {
        throw std::runtime_error("fail to write header");
    }

    swsCtx = sws_getContext(width,
                            height,
                            AV_PIX_FMT_RGB24,
                            width,
                            height,
                            AV_PIX_FMT_YUV420P,
                            SWS_BILINEAR,
                            nullptr,
                            nullptr,
                            nullptr);
    if (!swsCtx) {
        throw std::runtime_error("fail to create swsCtx");
    }

    frame = av_frame_alloc();
    if (!frame) {
        throw std::runtime_error("fail to create frame");
    }

    frame->format = AV_PIX_FMT_YUV420P;
    frame->width = width;
    frame->height = height;

    av_frame_get_buffer(frame, 32);

    packet = av_packet_alloc();
    if (!packet) {
        throw std::runtime_error("fail to create packet");
    }
}

void Mp4Creator::addFrame(const QImage &image) {
    QImage rgbImage = image.convertToFormat(QImage::Format_RGB888);
    uint8_t *inData[1] = {(uint8_t *) rgbImage.bits()};
    int inLinesize[1] = {(int) rgbImage.bytesPerLine()};

    sws_scale(swsCtx, inData, inLinesize, 0, codecCtx->height, frame->data, frame->linesize);

    frame->pts = pts += 100000;

    if (avcodec_send_frame(codecCtx, frame) < 0) {
        throw std::runtime_error("Error sending frame to encoder");
    }

    while (avcodec_receive_packet(codecCtx, packet) == 0) {
        packet->stream_index = stream->index;
        av_interleaved_write_frame(formatCtx, packet);
        av_packet_unref(packet);
    }
}

void Mp4Creator::destroy()
{
    // flush
    avcodec_send_frame(codecCtx, nullptr);
    while (avcodec_receive_packet(codecCtx, packet) == 0) {
        packet->stream_index = stream->index;
        av_interleaved_write_frame(formatCtx, packet);
        av_packet_unref(packet);
    }

    av_write_trailer(formatCtx);

    if (packet) {
        av_packet_free(&packet);
    }
    if (frame) {
        av_frame_free(&frame);
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    if (codecCtx) {
        avcodec_free_context(&codecCtx);
    }
    if (!(formatCtx->oformat->flags & AVFMT_NOFILE) && formatCtx->pb) {
        avio_closep(&formatCtx->pb);
    }
    if (formatCtx) {
        avformat_free_context(formatCtx);
    }
}
