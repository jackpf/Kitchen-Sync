#include "AudioFile.h"
#include "FLACDecoder.h"

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

static void wwh(FILE *f, FLACDecoder *decoder) {
    const FLAC__uint32 total_size = (FLAC__uint32)(decoder->getNumSamples() * decoder->getNumChannels() * (decoder->getBytesPerSample()/8));

    if(
        fwrite("RIFF", 1, 4, f) < 4 ||
        !write_little_endian_uint32(f, total_size + 36) ||
        fwrite("WAVEfmt ", 1, 8, f) < 8 ||
        !write_little_endian_uint32(f, 16) ||
        !write_little_endian_uint16(f, 1) ||
        !write_little_endian_uint16(f, (FLAC__uint16)decoder->getNumChannels()) ||
        !write_little_endian_uint32(f, decoder->getSampleRate()) ||
        !write_little_endian_uint32(f, decoder->getSampleRate() * decoder->getNumChannels() * (decoder->getBytesPerSample()/8)) ||
        !write_little_endian_uint16(f, (FLAC__uint16)(decoder->getNumChannels() * (decoder->getBytesPerSample()/8))) || /* block align */
        !write_little_endian_uint16(f, (FLAC__uint16)decoder->getBytesPerSample()) ||
        fwrite("data", 1, 4, f) < 4 ||
        !write_little_endian_uint32(f, total_size)
    ) {
        fprintf(stderr, "ERROR: write error\n");
    }
}

int main(int argc, char *argv[])
{
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <infile.flac> <outfile.wav>\n", argv[0]);
		return -1;
	}

	FLACDecoder decoder(argv[1]);

    fprintf(stderr, "decoding: %s\n", true? "succeeded" : "FAILED");
    fprintf(stderr, "   state: %s\n", decoder.get_state().resolved_as_cstring(decoder));

    float buf[6720];

    FILE *fh = fopen(argv[2], "w");
    wwh(fh, &decoder);

    while (decoder.eof() == 0) {
        int read = decoder.read(buf, 6720);
        fprintf(stderr, "Read %d samples\n", read);
        for (int i = 0; i < read; i++) {
            FLAC__uint16 d = (FLAC__uint16) buf[i];
            fputc(d, fh);
            fputc(d >> 8, fh);
        }
    }

    fclose(fh);

	return 0;
}