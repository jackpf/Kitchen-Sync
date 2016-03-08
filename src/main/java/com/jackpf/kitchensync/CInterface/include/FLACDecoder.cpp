#include "FLACDecoder.h"

FLACDecoder::FLACDecoder(const char *filename) : FLAC::Decoder::File() {
    this->filename = filename;

    set_md5_checking(true);

    FLAC__StreamDecoderInitStatus init_status = init(filename);
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        throw std::runtime_error(std::string("ERROR: initializing decoder: ") + std::string(FLAC__StreamDecoderInitStatusString[init_status]));
    }

    process_until_end_of_stream();
}

void FLACDecoder::write_little_endian_uint16_x2(FLAC__uint16 x1, FLAC__uint16 x2) {
    data.push_back((short) x1);
    data.push_back((short) x2);
}

int FLACDecoder::eof() const {
    return ptr < data.size() ? 0 : 1;
}

int FLACDecoder::read(float *buf, int len) {
    double conv = 1.0 / 32768.0;
    //assert(sizeof(int) == 4);

    int i;
    for (i = 0; i < len && eof() != 1; i++, ptr++) {
        buf[i] = (float) (_swap16(data.at(ptr)) * conv);
    }
    return i;
}

uint FLACDecoder::getNumChannels() const {
    return (uint) channels;
}

uint FLACDecoder::getSampleRate() const {
    return (uint) sampleRate;
}

uint FLACDecoder::getBytesPerSample() const {
    return (uint) getNumChannels() * getNumBits() / 8;
}

uint FLACDecoder::getNumSamples() const {
    return (uint) totalSamples;
}

uint FLACDecoder::getNumBits() const {
    return (uint) bps;
}

void FLACDecoder::rewind() {
    ptr = 0;
}

::FLAC__StreamDecoderWriteStatus FLACDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]) {
    size_t i;

    if(totalSamples == 0) {
        //fprintf(stderr, "Error: no sample_count in STREAMINFO\n");
        //return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        throw std::runtime_error("No sample_count in STREAMINFO\n");
    }
    if(channels != 2 || bps != 16) {
        std::string channelsStr;

        if (channels == 1) {
            channelsStr = "mono";
        } else if (channels == 2) {
            channelsStr = "stereo";
        } else {
            channelsStr = "unknown";
        }

        //fprintf(stderr, "Error: Audio must be 16 bit stereo, is: %d bit %s\n", bps, channelsStr);
        //return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        throw std::runtime_error(std::string("Audio must be 16 bit stereo, is: ") + std::to_string(bps) + std::string("bit ") + channelsStr);
    }

    /* write decoded PCM samples */
    for(i = 0; i < frame->header.blocksize; i++) {
        write_little_endian_uint16_x2((FLAC__uint16) buffer[0][i], (FLAC__uint16) buffer[1][i]);
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FLACDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata) {
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        /* save for later */
        totalSamples = metadata->data.stream_info.total_samples;
        sampleRate = metadata->data.stream_info.sample_rate;
        channels = metadata->data.stream_info.channels;
        bps = metadata->data.stream_info.bits_per_sample;
    }
}

void FLACDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status) {
    fprintf(stderr, "FLACDecoder error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}
