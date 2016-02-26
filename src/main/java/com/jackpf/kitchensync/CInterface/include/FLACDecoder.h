#ifndef __Kitchen_Sync_FLAC_Decoder_h__
#define __Kitchen_Sync_FLAC_Decoder_h__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "FLAC++/decoder.h"
#include "share/compat.h"
#include "AudioFile.h"

class FLACDecoder: public FLAC::Decoder::File, public AudioInFile {
public:
    FLACDecoder(const char *filename);

    int eof() const;
    int read(float *buf, int len);
    uint getNumChannels() const;
    uint getSampleRate() const;
    uint getBytesPerSample() const;
    uint getNumSamples() const;

protected:
    FLAC__uint64 total_samples  = 0;
    unsigned int sample_rate    = 0;
    unsigned int channels       = 0;
    unsigned int bps            = 0;

    std::vector<FLAC__uint16> data;
    unsigned int ptr = 0;

    void write_little_endian_uint16(FLAC__uint16 x);

    ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
    void metadata_callback(const ::FLAC__StreamMetadata *metadata);
    void error_callback(::FLAC__StreamDecoderErrorStatus status);
};

#endif