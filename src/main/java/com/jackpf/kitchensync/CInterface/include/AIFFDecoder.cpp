#include "AIFFDecoder.h"

AIFFDecoder::AIFFDecoder(const char *filename) {
    this->filename = filename;

    if ((ref = AIFF_OpenFile(filename, F_RDONLY)) == NULL) {
        throw std::runtime_error(std::string("Unable to open ") + filename);
    }

    if (AIFF_GetAudioFormat(ref,
                &totalSamples, &channels,
                &sampleRate, &bps,
                &segmentSize) < 1) {
        throw std::runtime_error(std::string("Unable to get audio format for ") + filename);
    }
}

int AIFFDecoder::eof() const {
    return sampleCount <= 0;
}

int AIFFDecoder::read(float *buf, int len) {
    sampleCount = AIFF_ReadSamplesFloat(ref, buf, len);
    return sampleCount;
}

uint AIFFDecoder::getNumChannels() const {
    return (uint) channels;
}

uint AIFFDecoder::getSampleRate() const {
    return (uint) sampleRate;
}

uint AIFFDecoder::getBytesPerSample() const {
    return (uint) getNumChannels() * getNumBits() / 8;
}

uint AIFFDecoder::getNumSamples() const {
    return (uint) totalSamples;
}

uint AIFFDecoder::getNumBits() const {
    return (uint) bps;
}

void AIFFDecoder::rewind() {
    AIFF_Seek(ref, 0);
}
