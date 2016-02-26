#include "FLACDecoder.h"

FLACDecoder::FLACDecoder(const char *filename) : FLAC::Decoder::File() {
    set_md5_checking(true);

    FLAC__StreamDecoderInitStatus init_status = init(filename);
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        throw std::runtime_error(std::string("ERROR: initializing decoder: ") + std::string(FLAC__StreamDecoderInitStatusString[init_status]));
    }

    process_until_end_of_stream();
}

void FLACDecoder::write_little_endian_uint16(FLAC__uint16 x)
{
    data.push_back(x);
}

int FLACDecoder::eof() const {
    return ptr < data.size() ? 0 : 1;
}

int FLACDecoder::read(float *buf, int len) {
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
    return (uint) sample_rate;
}

uint FLACDecoder::getBytesPerSample() const {
    return (uint) bps;
}

uint FLACDecoder::getNumSamples() const {
    return (uint) data.size();
}

::FLAC__StreamDecoderWriteStatus FLACDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
    size_t i;

    if(total_samples == 0) {
        fprintf(stderr, "ERROR: this example only works for FLAC files that have a total_samples count in STREAMINFO\n");
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    if(channels != 2 || bps != 16) {
        fprintf(stderr, "ERROR: this example only supports 16bit stereo streams\n");
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    /* write decoded PCM samples */
    for(i = 0; i < frame->header.blocksize; i++) {
        write_little_endian_uint16((FLAC__uint16)buffer[0][i]);  /* left channel */
        write_little_endian_uint16((FLAC__uint16)buffer[1][i]);  /* right channel */
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FLACDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
{
    /* print some stats */
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        /* save for later */
        total_samples = metadata->data.stream_info.total_samples;
        sample_rate = metadata->data.stream_info.sample_rate;
        channels = metadata->data.stream_info.channels;
        bps = metadata->data.stream_info.bits_per_sample;

        fprintf(stderr, "sample rate    : %u Hz\n", sample_rate);
        fprintf(stderr, "channels       : %u\n", channels);
        fprintf(stderr, "bits per sample: %u\n", bps);
        fprintf(stderr, "total samples  : %llu\n", total_samples);
    }
}

void FLACDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
    fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}
