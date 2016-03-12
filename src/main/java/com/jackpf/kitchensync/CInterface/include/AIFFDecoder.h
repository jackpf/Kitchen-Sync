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
    int sampleCount;

    void readFormat();

public:
    AIFFDecoder(const char *filename);

    int eof() const;
    int read(float *buf, int len);
    void rewind();
};

#endif