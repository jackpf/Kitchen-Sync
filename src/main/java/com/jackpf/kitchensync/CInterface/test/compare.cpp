#include "../include/WavFile.h"
#include "../include/FLACDecoder.h"
#include "../include/AudioFile.h"

#include <iostream>

#define LEN 10000

void printInfo(const char *filename, AudioInFile *inFile) {
    std::cerr << filename << ":\n\t" <<
        "channels=" << inFile->getNumChannels() <<
        ", sampleRate=" << inFile->getSampleRate() <<
        ", bytesPerSample=" << inFile->getBytesPerSample() <<
        ", numSamples=" << inFile->getNumSamples() <<
    std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Incorrect args" << std::endl;
        return -1;
    }

    std::cerr << "Comparing: \"" << argv[1] << "\" = \"" << argv[2] << "\"" << std::endl;

    AudioInFile *f1 = AudioFileFactory::createAudioInFile(argv[1]);
    AudioInFile *f2 = AudioFileFactory::createAudioInFile(argv[2]);

    printInfo(argv[1], f1);
    printInfo(argv[2], f2);

    float b1[LEN], b2[LEN];

    f1->read(b1, LEN);
    f2->read(b2, LEN);

    for (int i = 0; i < LEN; i++) {
        std::cerr << i << ": " << b1[i] << " = " << b2[i] << std::endl;
    }

    return 0;
}