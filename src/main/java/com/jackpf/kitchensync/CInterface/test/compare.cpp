#include "../include/WavFile.h"
#include "../include/FLACDecoder.h"
#include "../include/AudioFile.h"

#include <iostream>

using namespace std;

void printInfo(const char *filename, AudioInFile *inFile) {
    cerr << filename << ":\n\t" <<
        "channels=" << inFile->getNumChannels() <<
        ", sampleRate=" << inFile->getSampleRate() <<
        ", bytesPerSample=" << inFile->getBytesPerSample() <<
        ", numSamples=" << inFile->getNumSamples() <<
    endl;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <file1> <file2> <frameStart> <frameCount>" << endl;
        return -1;
    }
    
    string file1 = argv[1],
        file2 = argv[2];
    unsigned int startFrame = atoi(argv[3]),
        frameCount = atoi(argv[4]),
        endFrame = startFrame + frameCount;

    cerr << "Comparing: \"" << file1 << "\" = \"" << file2 << "\"" << endl;

    AudioInFile *f1 = AudioFileFactory::createAudioInFile(file1.c_str());
    AudioInFile *f2 = AudioFileFactory::createAudioInFile(file2.c_str());

    printInfo(file1.c_str(), f1);
    printInfo(file2.c_str(), f2);

    float b1[endFrame], b2[endFrame];

    f1->read(b1, endFrame);
    f2->read(b2, endFrame);

    for (int i = startFrame; i < endFrame; i++) {
        cerr << i << ": " << b1[i] << " = " << b2[i] << endl;
    }

    return 0;
}