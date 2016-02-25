#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <FLAC++/decoder.h>
#include <share/compat.h>

static FLAC__uint64 total_samples = 0;
static unsigned sample_rate = 0;
static unsigned channels = 0;
static unsigned bps = 0;

static bool write_little_endian_uint16(FILE *f, FLAC__uint16 x)
{
	return
		fputc(x, f) != EOF &&
		fputc(x >> 8, f) != EOF
	;
}

static bool write_little_endian_int16(FILE *f, FLAC__int16 x)
{
	return write_little_endian_uint16(f, (FLAC__uint16)x);
}

static bool write_little_endian_uint32(FILE *f, FLAC__uint32 x)
{
	return
		fputc(x, f) != EOF &&
		fputc(x >> 8, f) != EOF &&
		fputc(x >> 16, f) != EOF &&
		fputc(x >> 24, f) != EOF
	;
}

FLACDecoder::FLACDecoder(const char *filename) {
    set_md5_checking(true);
}

::FLAC__StreamDecoderWriteStatus OurDecoder::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 *const buffer[])
{
	const FLAC__uint32 total_size = (FLAC__uint32) (total_samples * channels * (bps / 8));
	size_t i;

	if(total_samples == 0) {
	    throw std::runtime_error("No total_samples count in STREAMINFO");
	}
	if(channels != 2 || bps != 16) {
	    throw std::runtime_error("Only 16 bit stereo streams supported");
	}

	for(i = 0; i < frame->header.blocksize; i++) {
		if(
			!write_little_endian_int16(f, (FLAC__int16) buffer[0][i]) ||  /* left channel */
			!write_little_endian_int16(f, (FLAC__int16) buffer[1][i])     /* right channel */
		) {
			fprintf(stderr, "ERROR: write error\n");
			return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
		}
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void OurDecoder::metadata_callback(const ::FLAC__StreamMetadata *metadata)
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
		fprintf(stderr, "total samples  : %llu \n", total_samples);
	}
}

void OurDecoder::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
	fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "usage: %s infile.flac\n", argv[0]);
		return 1;
	}

	FLACDecoder decoder;

    FLAC__StreamDecoderInitStatus status = decoder.init(filename);

    if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        throw std::runtime_error(std::string("ERROR: initializing decoder: ") + std::string(FLAC__StreamDecoderInitStatusString[init_status]));
    }

    if (decoder.process_until_end_of_stream()) {
        fprintf(stderr, "decoding: succeeded\n");
        fprintf(stderr, "   state: %s\n", decoder.get_state().resolved_as_cstring(decoder));
    } else {
        throw std::runtime_error("Decoding failed");
    }

	return 0;
}