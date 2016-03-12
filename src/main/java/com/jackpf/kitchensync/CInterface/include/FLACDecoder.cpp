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
    if (bps == 8) {
        unsigned char v = (unsigned char) x;
        data.push_back(v * this->getConv() - 1.0);
    } else if (bps == 16) {
        short v = (short) x;
        data.push_back(_swap16(v) * this->getConv());
    } else if (bps == 24) {
        int v = (int) x;
        data.push_back(_swap32(v) * this->getConv());
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

void FLACDecoder::rewind() {
    ptr = 0;
}

::FLAC__StreamDecoderWriteStatus FLACDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 *const buffer[]) {
    size_t i;

    if(this->getNumSamples() == 0) {
        throw std::runtime_error("No sample_count in STREAMINFO\n");
    }

    for(i = 0; i < frame->header.blocksize; i++) {
        writeBuffer(buffer[0][i], this->getNumBits());
        if (this->getNumChannels() == 2) {
            writeBuffer(buffer[1][i], this->getNumBits());
        }
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FLACDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata) {
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        this->setNumSamples((uint) metadata->data.stream_info.total_samples);
        this->setSampleRate((uint) metadata->data.stream_info.sample_rate);
        this->setNumChannels((uint) metadata->data.stream_info.channels);
        this->setNumBits((uint) metadata->data.stream_info.bits_per_sample);
    }
}

void FLACDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status) {
    fprintf(stderr, "FLACDecoder error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}
