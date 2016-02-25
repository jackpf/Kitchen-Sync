#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

extern "C" {
    #include "libavutil/mathematics.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include <ao/ao.h>
}

#include <soundtouch/SoundTouch.h>
#include <soundtouch/BPMDetect.h>

using namespace soundtouch;

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

void setup(AVFormatContext **container, const char *filename) {
    av_register_all();

    *container = avformat_alloc_context();

    if (avformat_open_input(container, filename, NULL, NULL) < 0){
        throw std::runtime_error("Incorrect arguments");
    }

    if (avformat_find_stream_info(*container, NULL) < 0){
        throw std::runtime_error("Could not find file info");
    }

    av_dump_format(*container, 0, filename, false);
}

int findAudioStream(AVFormatContext *container) {
    for(int i = 0; i < container->nb_streams; i++) {
        if(container->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            return i;
        }
    }

    throw std::runtime_error("Could not find Audio Stream");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        throw std::runtime_error("Incorrect arguments");
    }

    AVFormatContext *container;
    AVCodecContext *context;
    AVCodec *codec;
    int streamId;

    setup(&container, argv[1]);
    streamId = findAudioStream(container);

    context = container->streams[streamId]->codec;
    codec = avcodec_find_decoder(context->codec_id);

    if (codec == NULL || avcodec_open2(context, codec, NULL) < 0) {
        throw std::runtime_error("Codec cannot be found");
    }

    AVPacket packet;
    AVFrame *frame;
    int bufSize;
    int frameFinished;

    bufSize = AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE;
    uint8_t buffer[bufSize];

    av_init_packet(&packet);
    frame = av_frame_alloc();

    packet.data = buffer;
    packet.size = bufSize;

    int nChannels = container->streams[streamId]->codec->channels;
    int sampleRate = container->streams[streamId]->codec->sample_rate;

    BPMDetect *bpmDetector = new BPMDetect(nChannels, sampleRate);

    FILE *outfile = fopen("/Users/jackfarrelly/out.pcm", "w");

    while (av_read_frame(container, &packet) >= 0) {
        if (packet.stream_index == streamId) {
            int len;
            int samples;

            len = avcodec_decode_audio4(context, frame, &frameFinished, &packet);

            if (frameFinished) {
                for (int i = 0; i < frame->nb_samples; i++) {
                    //printf("Sample: L=%d, R=%d\n", frame->data[0][i], frame->data[1][i]);
                }

                //samples = len / nChannels;

                //bpmDetector->inputSamples(frame->data[0], samples);
                int data_size = av_get_bytes_per_sample(context->sample_fmt);
                if (data_size < 0) {
                    /* This should not occur, checking just for paranoia */
                    fprintf(stderr, "Failed to calculate data size\n");
                    exit(1);
                }
                for (int i=0; i<frame->nb_samples; i++)
                    for (int ch=0; ch<context->channels; ch++)
                        fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
            } else {
                printf("Not Finished\n");
            }

        }
    }

    //printf("BPM: %f\n", bpmDetector->getBpm());

    avformat_close_input(&container);

    return 0;
}