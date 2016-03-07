#ifndef __Kitchen_Sync_FLAC_Decoder_h__
#define __Kitchen_Sync_FLAC_Decoder_h__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "FLAC++/decoder.h"
#include "share/compat.h"
#include "AudioFile.h"

#ifdef BYTE_ORDER
    #if BYTE_ORDER == BIG_ENDIAN
        #define _BIG_ENDIAN_
    #endif
#endif

#ifdef _BIG_ENDIAN_
    static inline int _swap32(int &dwData) {
        dwData = ((dwData >> 24) & 0x000000FF) |
               ((dwData >> 8)  & 0x0000FF00) |
               ((dwData << 8)  & 0x00FF0000) |
               ((dwData << 24) & 0xFF000000);
        return dwData;
    }

    static inline short _swap16(short &wData) {
        wData = ((wData >> 8) & 0x00FF) |
                ((wData << 8) & 0xFF00);
        return wData;
    }

    static inline void _swap16Buffer(short *pData, int numWords) {
        int i;

        for (i = 0; i < numWords; i ++) {
            pData[i] = _swap16(pData[i]);
        }
    }
#else
    static inline int _swap32(int &dwData) {
        return dwData;
    }

    static inline short _swap16(short &wData) {
        return wData;
    }

    static inline void _swap16Buffer(short *pData, int numBytes) {
    }
#endif

class FLACDecoder: public FLAC::Decoder::File, public AudioInFile {
protected:
    const char *filename;

    FLAC__uint64 total_samples  = 0;
    unsigned int sample_rate    = 0;
    unsigned int channels       = 0;
    unsigned int bps            = 0;

    std::vector<short> data;
    unsigned int ptr = 0;

    void write_little_endian_uint16_x2(FLAC__uint16 x1, FLAC__uint16 x2);

    ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
    void metadata_callback(const ::FLAC__StreamMetadata *metadata);
    void error_callback(::FLAC__StreamDecoderErrorStatus status);

public:
    FLACDecoder(const char *filename);

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