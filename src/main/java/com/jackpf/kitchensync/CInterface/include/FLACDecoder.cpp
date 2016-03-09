#include "FLACDecoder.h"

FLACDecoder::FLACDecoder(const char *filename) : FLAC::Decoder::File() {
    this->filename = filename;

    set_md5_checking(true);

    FLAC__StreamDecoderInitStatus init_status = init(filename);
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        throw std::runtime_error(std::string("FLACDecoder error: Unable to initialise decoder: ") + std::string(FLAC__StreamDecoderInitStatusString[init_status]));
    }

    process_until_end_of_stream();
}

void FLACDecoder::writeBuffer(FLAC__int32 x, int bps) {
    if (bps == 16) {
        double conv = 1.0 / 32768.0;
        short v = (short) x;
        data.push_back(_swap16(v) * conv);
    } else if (bps == 24) {
        double conv = 1.0 / 2147483648.0;
        int v = (int) x;
        data.push_back(_swap32(v) * conv);
    } else {
        throw std::runtime_error(std::string("FLACDecoder error: Unsupported bit depth of ") + std::to_string(bps));
    }
}

int FLACDecoder::eof() const {
    return ptr < data.size() ? 0 : 1;
}

int FLACDecoder::read(float *buf, int len) {
    //assert(sizeof(int) == 4);
    int i;
    for (i = 0; i < len && eof() != 1; i++, ptr++) {
        buf[i] = data.at(ptr);
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

::FLAC__StreamDecoderWriteStatus FLACDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 *const buffer[]) {
    size_t i;

    if(totalSamples == 0) {
        throw std::runtime_error("No sample_count in STREAMINFO\n");
    }

    for(i = 0; i < frame->header.blocksize; i++) {
        writeBuffer(buffer[0][i], bps);
        if (channels == 2) {
            writeBuffer(buffer[1][i], bps);
        }
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FLACDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata) {
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        totalSamples = metadata->data.stream_info.total_samples;
        sampleRate = metadata->data.stream_info.sample_rate;
        channels = metadata->data.stream_info.channels;
        bps = metadata->data.stream_info.bits_per_sample;
    }
}

void FLACDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status) {
    fprintf(stderr, "FLACDecoder error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}
