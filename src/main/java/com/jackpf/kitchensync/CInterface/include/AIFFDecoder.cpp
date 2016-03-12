#include "AIFFDecoder.h"

AIFFDecoder::AIFFDecoder(const char *filename) {
    this->filename = filename;

    if ((ref = AIFF_OpenFile(filename, F_RDONLY)) == NULL) {
        throw std::runtime_error(std::string("Unable to open ") + filename);
    }

    readFormat();
}

void AIFFDecoder::readFormat() {
    uint64_t totalSamples;
    double sampleRate;
    int channels;
    int bps;
    int segmentSize;

    if (AIFF_GetAudioFormat(ref,
                &totalSamples, &channels,
                &sampleRate, &bps,
                &segmentSize) < 1) {
        throw std::runtime_error(std::string("Unable to get audio format for ") + filename);
    }

    this->setNumSamples((uint) totalSamples);
    this->setSampleRate((uint) sampleRate);
    this->setNumChannels((uint) channels);
    this->setNumBits((uint) bps);
}

int AIFFDecoder::eof() const {
    return sampleCount <= 0;
}

int AIFFDecoder::read(float *buf, int len) {
    sampleCount = AIFF_ReadSamplesFloat(ref, buf, len);
    return sampleCount;
}

void AIFFDecoder::rewind() {
    AIFF_Seek(ref, 0);
}
