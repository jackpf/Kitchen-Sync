#include <jni.h>
#include <iostream>

#include "KitchenSync.h"
#include "KitchenSyncAnalyser.h"

static void throwException(const std::runtime_error &e) {
    const char pre[] = "Kitchen Sync error : ";
    char mBuf[strlen(pre) + strlen(e.what()) + 1];

    sprintf(mBuf, "%s%s", pre, e.what());

    printf("%s\n", mBuf);
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <inFile> <outFile> <targetBpm>\n", argv[0]);
        return -1;
    }

    try {
        RunParameters params;

        params.inFileName = argv[1];
        params.outFileName = argv[2];

        KitchenSync ks(&params);

        printf("Soundtouch lib version: %s\n", ks.getVersion());

        float bpm = ks.getBpm();
        printf("BPM of %s: %f'n\n", argv[1], bpm);

        printf("Quality of %s: %f\n", argv[1], ks.getQuality());

        ks.setBpm(bpm, std::stof(argv[3]));

        printf("Wrote to %s\n", argv[2]);
    } catch (const std::runtime_error &e) {
        throwException(e);
    }

    return 0;
}