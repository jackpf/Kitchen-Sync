#include <stdio.h>
#include <iostream>
#include <lame/lame.h>

#include "../include/WavFile.h"

using namespace std;

#define MP3_SIZE 8192
#define PCM_SIZE MP3_SIZE * 16

void printInfo(const char *filename, mp3data_struct *mp3data) {
    cerr << filename << ":\n\t" <<
        "channels=" << mp3data->stereo <<
        ", sampleRate=" << mp3data->samplerate <<
        ", frameSize=" << mp3data->framesize <<
        ", numSamples=" << mp3data->framesize * mp3data->totalframes <<
    endl;
}

int main(int argc, char *argv[])
{
    int read;
    int decoded;
    mp3data_struct mp3data;

    short pcm_buffer_l[PCM_SIZE];
    short pcm_buffer_r[PCM_SIZE];
    unsigned char mp3_buffer[MP3_SIZE];

    FILE *mp3 = fopen(argv[1], "r");
    hip_t hip = hip_decode_init();
    WavOutFile *outFile = nullptr;

    do {
        read = fread(mp3_buffer, sizeof(unsigned char), MP3_SIZE, mp3);

        decoded = hip_decode_headers(hip, mp3_buffer, MP3_SIZE, pcm_buffer_l, pcm_buffer_r, &mp3data);

        if (mp3data.header_parsed == 1 && outFile == nullptr) {
            printInfo(argv[1], &mp3data);
            outFile = new WavOutFile(argv[2], mp3data.samplerate, 16, mp3data.stereo);
        }

        if (outFile != nullptr) {
            short buf[decoded * 2];
            for (int i = 0, j = 0; i < decoded * 2; i += 2, j++) {
                buf[i] = pcm_buffer_l[j];
                buf[i + 1] = pcm_buffer_r[j];
            }
            outFile->write(buf, decoded * 2);
        }
    } while (read != 0);

    delete outFile;
    hip_decode_exit(hip);
    fclose(mp3);

    printf("done\n");

    return 0;
}