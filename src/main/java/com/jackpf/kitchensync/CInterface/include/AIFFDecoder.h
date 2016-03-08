#ifndef __Kitchen_Sync_AIFF_Decoder_h__
#define __Kitchen_Sync_AIFF_Decoder_h__

#include "AudioFile.h"
#include <iostream>

extern "C" {
    #define LIBAIFF_NOCOMPAT 1 // do not use LibAiff 2 API compatibility
    #include <libaiff/libaiff.h>
}

class AIFFDecoder : public AudioInFile {
protected:
    const char *filename;

    AIFF_Ref ref;

    uint64_t totalSamples   = 0;
    double sampleRate       = 0;
    int channels            = 0;
    int bps                 = 0;
    int sampleCount;
    int segmentSize;

public:
    AIFFDecoder(const char *filename);

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