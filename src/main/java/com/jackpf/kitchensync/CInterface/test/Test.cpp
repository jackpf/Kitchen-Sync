#include <jni.h>
#include <iostream>

#include "KitchenSync.h"
#include "KitchenSyncAnalyser.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: kstest <inFile> <outFile> <targetBpm>\n");
        return -1;
    }

    const char *version = KitchenSync::getVersion();
    printf("Soundtouch lib version: %s\n", version);

    float bpm = KitchenSync::getBpm(argv[1]);

    printf("BPM of %s: %f'n\n", argv[1], bpm);

    float quality = KitchenSync::getQuality(argv[1]);

    printf("Quality of %s: %f\n", argv[1], quality);

    KitchenSync::setBpm(argv[1], argv[2], bpm, std::stof(argv[3]));

    printf("Wrote to %s\n", argv[2]);

    return 0;
}