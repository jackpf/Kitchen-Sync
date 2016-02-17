#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>
extern "C" {
    #include <libavutil/opt.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/common.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/samplefmt.h>
}

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

AVCodecContext *cntx_;

AVCodec *codec_;

std::vector<unsigned char>  decode(unsigned char* data, unsigned int len)
{
    std::vector<unsigned char> ret;

    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = data;
    avpkt.size = len;

    AVFrame* pframe = av_frame_alloc();
    while (avpkt.size > 0){
        int goted = -1;av_frame_unref(pframe);
        int used = avcodec_decode_audio4(cntx_, pframe, &goted, &avpkt);
        if (goted){
            ret.insert(ret.end(), pframe->data[0], pframe->data[0] + pframe->linesize[0]);
            avpkt.data += used;
            avpkt.size -= used;
            avpkt.dts = avpkt.pts = AV_NOPTS_VALUE;
        }
        else if (goted == 0){
            avpkt.data += used;
            avpkt.size -= used;
            avpkt.dts = avpkt.pts = AV_NOPTS_VALUE;
        }
        else if(goted < 0){
            break;
        }
    }
    av_frame_free(&pframe);
    return ret;
}

int main(int argc, char **argv)
{
avcodec_register_all();

cntx_ = new AVCodecContext();
codec_ = avcodec_find_encoder(AV_CODEC_ID_MP2);

cntx_->channels = 1;
cntx_->sample_rate = 44100;
cntx_->sample_fmt = AV_SAMPLE_FMT_S16P;
cntx_->channel_layout =  AV_CH_LAYOUT_STEREO;
cntx_->bit_rate = 16000;
int err_ = avcodec_open2(cntx_, codec_, NULL);

    std::ifstream ifs(argv[1]);
      std::string content( (std::istreambuf_iterator<char>(ifs) ),
                           (std::istreambuf_iterator<char>()    ) );

    decode((unsigned char *)content.c_str(), content.length());

    return 0;
}