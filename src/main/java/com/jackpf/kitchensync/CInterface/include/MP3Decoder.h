#ifndef __Kitchen_Sync_MP3_Decoder_h__
#define __Kitchen_Sync_MP3_Decoder_h__

#include <iostream>
#include <vector>
#include <lame/lame.h>

#include "AudioFile.h"

#define MP3_SIZE 1024
#define PCM_SIZE MP3_SIZE * 1152
#define CONV 1.0 / 32768.0

class MP3Decoder : public AudioInFile {
protected:
    const char *filename;

    FILE *mp3;
    hip_t hip;
    mp3data_struct mp3data;
    std::vector<float> buffer;

    uint64_t totalSamples   = 0;
    double sampleRate       = 0;
    int channels            = 0;
    int bps                 = 0;
    int sampleCount;
    int segmentSize;

    void readHeaders();
    int decodeChunk();

public:
    MP3Decoder(const char *filename);
    ~MP3Decoder();

    int eof() const;
    int read(float *buf, int len);
    uint getNumChannels() const;
    uint getSampleRate() const;
    uint getBytesPerSample() const;
    uint getNumSamples() const;
    uint getNumBits() const;
    void rewind();
};

#endif