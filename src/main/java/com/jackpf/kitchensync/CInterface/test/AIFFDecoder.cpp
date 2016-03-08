#include <stdlib.h>
#include <iostream>

extern "C" {
    #define LIBAIFF_NOCOMPAT 1 // do not use LibAiff 2 API compatibility
    #include <libaiff/libaiff.h>
}

#define BUF_SIZE 6720

int main(int argc, char *argv[]) {
    char *filename;
    AIFF_Ref ref;
    uint64_t seconds, nSamples;
    int channels;
    double samplingRate;
    int bitsPerSample;
    int segmentSize;
    float buf[BUF_SIZE];
    int sampleCount;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.aiff>\n", argv[0]);
        exit(-1);
    }

    filename = argv[1];

    if((ref = AIFF_OpenFile(filename, F_RDONLY)) == NULL) {
    	fprintf(stderr, "Unable to open %s\n", filename);
        exit(-1);
    }

    if(AIFF_GetAudioFormat(ref, &nSamples, &channels,
    			&samplingRate, &bitsPerSample,
    			&segmentSize) < 1) {
    	fprintf(stderr, "Unable to get audio format for %s\n", filename);
        exit(-1);
    }

    seconds = nSamples / samplingRate ;
    /*
     * Print out the seconds in H:MM:SS format
     */
    printf("Length: %lu:%02lu:%02lu \n", (unsigned long) seconds/3600,
    	(unsigned long) (seconds/60)%60, (unsigned long) seconds%60 );

    std::cerr << filename << ":\n\t" <<
        "channels=" << channels <<
        ", sampleRate=" << samplingRate <<
        ", bytesPerSample=" << bitsPerSample / 8 <<
        ", numSamples=" << nSamples <<
        ", segmentSize=" << segmentSize <<
    std::endl;

    int c = 0;
    while ((sampleCount = AIFF_ReadSamplesFloat(ref, buf, BUF_SIZE)) > 0) {
        int i;
        for (i = 0; i < sampleCount; i++) {
            //printf("Sample %d: %f\n", c, buf[i]);
            c++;
        }
    }

    return 0;
}