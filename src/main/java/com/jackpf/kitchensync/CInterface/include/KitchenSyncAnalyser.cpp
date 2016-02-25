#include "KitchenSyncAnalyser.h"

KitchenSyncAnalyser::KitchenSyncAnalyser(const char *filename) :
    BUFF_SIZE(6720),
    MAX_FREQ(22),
    MIN_AN_FREQ(16),
    MAX_AN_FREQ(22) {
    this->filename = filename;
    inFile = new WavInFile(filename);
    frequencyMagnitudes = new float[MAX_FREQ + 1];

    fill_n(frequencyMagnitudes, MAX_FREQ + 1, 0.0);

    if (inFile->getNumChannels() > 1) {
        //throw new runtime_error("This version only works on mono files :(");
    }
}

KitchenSyncAnalyser::~KitchenSyncAnalyser() {
    delete inFile;
    delete[] frequencyMagnitudes;
}

void KitchenSyncAnalyser::printInfo() {
    cerr << filename << ":\n\t" <<
        "channels=" << inFile->getNumChannels() <<
        ", sampleRate=" << inFile->getSampleRate() <<
        ", bytesPerSample=" << inFile->getBytesPerSample() <<
        ", numSamples=" << inFile->getNumSamples() <<
    endl;
}

void KitchenSyncAnalyser::printMagnitudes() {
    for (int i = 0; i <= MAX_FREQ; i += 2) {
        fprintf(stderr, "%dKHz magnitude: %f\n", i, frequencyMagnitudes[i]);
    }
}

void KitchenSyncAnalyser::calculateFrequencies(fftw_complex *data, size_t len, int Fs) {
    for (int i = 0; i < len; i++) {
        int re, im;
        float freq, magnitude;
        int index;

        re = data[i][0];
        im = data[i][1];

        magnitude = sqrt(re * re + im * im);
        freq = i * Fs / len;

        index = floor(freq / 1000.0);
        if (index <= MAX_FREQ) {
            frequencyMagnitudes[index] += magnitude;
        }
    }
}

void KitchenSyncAnalyser::calculateFrequencyMagnitudes() {
    assert(inFile->getNumChannels() == 1 || inFile->getNumChannels() == 2);
    SAMPLETYPE sampleBuffer[BUFF_SIZE * inFile->getNumChannels()];

    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * BUFF_SIZE);
    out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * BUFF_SIZE);

    p = fftw_plan_dft_1d(BUFF_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    while (inFile->eof() == 0) {
        size_t samplesRead = inFile->read(sampleBuffer, BUFF_SIZE * inFile->getNumChannels());

        for (int i = 0, j = 0; j < samplesRead; i++, j += inFile->getNumChannels()) {
            // Convert to mono if necessary
            if (inFile->getNumChannels() == 2) {
                in[i][0] = (double) (sampleBuffer[j] + sampleBuffer[j + 1]) / 2.0;
            } else {
                in[i][0] = (double) sampleBuffer[j];
            }

            in[i][1] = 0.0; // Fuck's sake
        }

        fftw_execute(p); /* repeat as needed */

        calculateFrequencies(out, samplesRead / inFile->getNumChannels(), inFile->getSampleRate());
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}

float KitchenSyncAnalyser::analyse() {
    int score = 0;
    int max = 0;
    int threshold = 0;

    calculateFrequencyMagnitudes();

    for (int i = MIN_AN_FREQ; i <= MAX_AN_FREQ; i += 2) {
        if (frequencyMagnitudes[i] > threshold) {
            score++;
        }
        max++;
    }

    return (float) score / max;
}