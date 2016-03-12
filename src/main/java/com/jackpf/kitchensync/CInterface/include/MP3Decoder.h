#ifndef __Kitchen_Sync_MP3_Decoder_h__
#define __Kitchen_Sync_MP3_Decoder_h__

#include <iostream>
#include <vector>
#include <lame/lame.h>

#include "AudioFile.h"

#define MP3_SIZE 1024
#define PCM_SIZE MP3_SIZE * 1152

class MP3Decoder : public AudioInFile {
protected:
    const char *filename;

    FILE *mp3;
    hip_t hip;
    mp3data_struct mp3data;
    std::vector<float> buffer;

    void readHeaders();
    int decodeChunk();

public:
    MP3Decoder(const char *filename);
    ~MP3Decoder();

    int eof() const;
    int read(float *buf, int len);
    void rewind();
};

#endif