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

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

void die(const char *msg)
{
    fprintf(stderr,"%s\n",msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    const char* input_filename=argv[1];

    //avcodec_register_all();
    av_register_all();
    //av_ini

    AVFormatContext* container=avformat_alloc_context();
    if(avformat_open_input(&container,input_filename,NULL,NULL)<0){
        die("Could not open file");
    }

    if(avformat_find_stream_info(container, NULL)<0){
        die("Could not find file info");
    }
    av_dump_format(container,0,input_filename,false);

    int stream_id=-1;
    int i;
    for(i=0;i<container->nb_streams;i++){
        if(container->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO){
            stream_id=i;
            break;
        }
    }
    if(stream_id==-1){
        die("Could not find Audio Stream");
    }

    AVDictionary *metadata=container->metadata;

    AVCodecContext *ctx=container->streams[stream_id]->codec;
    AVCodec *codec=avcodec_find_decoder(ctx->codec_id);

    if(codec==NULL){
        die("cannot find codec!");
    }

    if(avcodec_open2(ctx,codec, NULL)<0){
        die("Codec cannot be found");
    }

    //ctx=avcodec_alloc_context3(codec);

    //initialize AO lib
    ao_initialize();

    int driver=ao_default_driver_id();

    ao_sample_format sformat;
    AVSampleFormat sfmt=ctx->sample_fmt;
    if(sfmt==AV_SAMPLE_FMT_U8){
        printf("U8\n");

        sformat.bits=8;
    }else if(sfmt==AV_SAMPLE_FMT_S16){
        printf("S16\n");
        sformat.bits=16;
    }else if(sfmt==AV_SAMPLE_FMT_S32){
        printf("S32\n");
        sformat.bits=32;
    }

    sformat.channels=ctx->channels;
    sformat.rate=ctx->sample_rate;
    sformat.byte_format=AO_FMT_NATIVE;
    sformat.matrix=0;

    ao_device *adevice=ao_open_live(driver,&sformat,NULL);
    //end of init AO LIB

    AVPacket packet;
    av_init_packet(&packet);

    AVFrame *frame=av_frame_alloc();



    int buffer_size=AVCODEC_MAX_AUDIO_FRAME_SIZE+ FF_INPUT_BUFFER_PADDING_SIZE;;
    uint8_t buffer[buffer_size];
    packet.data=buffer;
    packet.size =buffer_size;



    int len;
    int frameFinished=0;
    while(av_read_frame(container,&packet)>=0)
    {
        //printf("%d = %d\n", packet.stream_index, stream_id);
        if(packet.stream_index==stream_id){
            //printf("Audio Frame read  \n");
            int len=avcodec_decode_audio4(ctx,frame,&frameFinished,&packet);
            //frame->
            if(frameFinished){
                //printf("Frame: %s\n", frame->extended_data[0]);
                //printf("Finished reading Frame len : %d , nb_samples:%d buffer_size:%d line size: %d \n",len,frame->nb_samples,buffer_size,frame->linesize[0]);
                //ao_play(adevice, (char*)frame->extended_data[0],frame->linesize[0] );
                printAudioFrameInfo(ctx, frame);
            }else{
                printf("Not Finished\n");
            }

        }else {
            printf("Someother packet possibly Video\n");
        }


    }

    avformat_close_input(&container);
    ao_shutdown();
    return 0;
}