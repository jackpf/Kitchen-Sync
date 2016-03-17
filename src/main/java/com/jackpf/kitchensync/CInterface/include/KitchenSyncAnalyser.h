#ifndef __Kitchen_Sync_Analyser_h__
#define __Kitchen_Sync_Analyser_h__

#include <iostream>
#include <math.h>

#include <fftw3.h>
#include <soundtouch/SoundTouch.h>
#include "WavFile.h"
#include "FLACDecoder.h"
#include "AudioFile.h"

using namespace std;
using namespace soundtouch;

class KitchenSyncAnalyser {
private:
    const char *filename;
    AudioInFile *inFile;
    double *frequencyMagnitudes;

    void calculateFrequencies(fftw_complex *data, size_t len, int Fs);
    void calculateFrequencyMagnitudes();

public:
    const int BUFF_SIZE;
    const int MAX_FREQ;
    const int MIN_AN_FREQ;
    const int MAX_AN_FREQ;

    KitchenSyncAnalyser(const char *filename);
    ~KitchenSyncAnalyser();

    void printInfo();
    void printMagnitudes();
    float analyse();
};

#endif
