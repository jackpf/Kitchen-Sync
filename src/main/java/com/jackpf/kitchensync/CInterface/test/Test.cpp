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
        fprintf(stderr, "Usage: kstest <inFile> <outFile> <targetBpm>\n");
        return -1;
    }

    try {
        const char *version = KitchenSync::getVersion();
        printf("Soundtouch lib version: %s\n", version);

        float bpm = KitchenSync::getBpm(argv[1]);

        printf("BPM of %s: %f'n\n", argv[1], bpm);

        float quality = KitchenSync::getQuality(argv[1]);

        printf("Quality of %s: %f\n", argv[1], quality);

        KitchenSync::setBpm(argv[1], argv[2], bpm, std::stof(argv[3]));

        printf("Wrote to %s\n", argv[2]);
    } catch (const std::runtime_error &e) {
        throwException(e);
    }

    return 0;
}