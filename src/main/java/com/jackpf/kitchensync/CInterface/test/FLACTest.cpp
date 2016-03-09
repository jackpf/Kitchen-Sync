#include "AudioFile.h"
#include "FLACDecoder.h"
#include "WavFile.h"

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

    WavOutFile outFile(argv[2], decoder.getSampleRate(), decoder.getNumBits(), decoder.getNumChannels());

    while (decoder.eof() == 0) {
        int read = decoder.read(buf, 6720);
        outFile.write(buf, read);
    }

    printf("Done\n");

	return 0;
}