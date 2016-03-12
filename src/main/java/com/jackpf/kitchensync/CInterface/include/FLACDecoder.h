#ifndef __Kitchen_Sync_FLAC_Decoder_h__
#define __Kitchen_Sync_FLAC_Decoder_h__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "FLAC++/decoder.h"
#include "share/compat.h"
#include "AudioFile.h"

class FLACDecoder : public FLAC::Decoder::File, public AudioInFile {
protected:
    const char *filename;

    std::vector<float> data;
    unsigned int ptr = 0;

    void writeBuffer(FLAC__int32 x, int bps);

    ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
    void metadata_callback(const ::FLAC__StreamMetadata *metadata);
    void error_callback(::FLAC__StreamDecoderErrorStatus status);

public:
    FLACDecoder(const char *filename);

    int eof() const;
    int read(float *buf, int len);
    void rewind();
};

#endif