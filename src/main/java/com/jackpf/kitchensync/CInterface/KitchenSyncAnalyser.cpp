#include "KitchenSyncAnalyser.h"

#include <iostream>
#include <math.h>

#include <fftw3.h>
#include <soundtouch/SoundTouch.h>
#include "include/WavFile.h"

using namespace std;
using namespace soundtouch;

#define BUFF_SIZE           6720
#define MAX_FREQ            22//KHz

static void calculateFrequencies(fftw_complex *data, size_t len, int Fs, float *frequencyMagnitudes) {
    for (int i = 0; i < len; i++) {
        int re, im;
        float freq, magnitude;
        int index;

        re = data[i][0];
        im = data[i][1];

        magnitude = sqrt(re * re + im * im);
        freq = i * Fs / len;

        index = round(freq / 1000.0);
        if (index <= MAX_FREQ) {
            frequencyMagnitudes[index] += magnitude;
        }
    }
}

void calculateFrequencyMagnitudes(const char *filename, float *frequencyMagnitudes) {
    SAMPLETYPE sampleBuffer[BUFF_SIZE];
    WavInFile inFile(filename);

    cout << filename << ":\n\t" <<
        "channels=" << inFile.getNumChannels() <<
        ", sampleRate=" << inFile.getSampleRate() <<
        ", bytesPerSample=" << inFile.getBytesPerSample() <<
        ", numSamples=" << inFile.getNumSamples() <<
    endl;

    if (inFile.getNumChannels() > 1) {
        cerr << "This version only works on mono files :(" << endl;
        exit(-1);
    }

    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * BUFF_SIZE);
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * BUFF_SIZE);

    p = fftw_plan_dft_1d(BUFF_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    while (inFile.eof() == 0) {
        size_t samplesRead = inFile.read(sampleBuffer, BUFF_SIZE);

        for (int i = 0; i < BUFF_SIZE; i++) {
            in[i][0] = (double) sampleBuffer[i];
            in[i][1] = 0.0; // Fuck's sake
        }

        fftw_execute(p); /* repeat as needed */

        calculateFrequencies(out, samplesRead, inFile.getSampleRate(), frequencyMagnitudes);
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

 float analyse(const char *filename) {
    float fm[MAX_FREQ + 1] = {0};
    int score = 0, max = 0;
    int threshhold = 1000; // This should be some kind of percentage of the sum of the magnitudes

    calculateFrequencyMagnitudes(filename, fm);

    for (int i = 0; i <= MAX_FREQ; i += 2) {
        fprintf(stderr, "%dKHz magnitude: %f\n", i, fm[i]);
    }

    for (int i = 16; i <= 20; i += 2) {
        if (fm[i] > threshhold) {
            score++;
        }
        max++;
    }

    return (float) score / max;
}