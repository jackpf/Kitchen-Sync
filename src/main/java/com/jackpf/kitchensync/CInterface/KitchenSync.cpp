#include "KitchenSync.h"

KitchenSync::KitchenSync(RunParameters *params) : BUFF_SIZE(6720) {
    this->soundTouch = new SoundTouch();
    this->params = params;

    openFiles();
}

KitchenSync::~KitchenSync() {
    delete soundTouch;
}

void KitchenSync::openFiles() {
    int bits, samplerate, channels;

    assert(params->inFileName != nullptr);
    inFile = new WavInFile(params->inFileName);

    bits = (int) inFile->getNumBits();
    samplerate = (int) inFile->getSampleRate();
    channels = (int) inFile->getNumChannels();

    if (params->outFileName != nullptr) {
        outFile = new WavOutFile(params->outFileName, samplerate, bits, channels);
    }
}

const char *KitchenSync::getVersion() {
    return soundTouch->getVersionString();
}

void KitchenSync::printInfo() {
    if (params->outFileName) {
    #ifdef SOUNDTOUCH_INTEGER_SAMPLES
        fprintf(stderr, "Uses 16bit integer sample type in processing.\n\n");
    #else
        #ifndef SOUNDTOUCH_FLOAT_SAMPLES
            #error "Sampletype not defined"
        #endif
        fprintf(stderr, "Uses 32bit floating point sample type in processing.\n\n");
    #endif
        if (params->speech) {
            fprintf(stderr, "Tune processing parameters for speech processing.\n");
        }
        // print processing information only if outFileName given i.e. some processing will happen
        fprintf(stderr, "Processing the file with the following changes:\n");
        fprintf(stderr, "  tempo change = %+g %%\n", params->tempoDelta);
        fprintf(stderr, "  pitch change = %+g semitones\n", params->pitchDelta);
        fprintf(stderr, "  rate change  = %+g %%\n\n", params->rateDelta);
        fprintf(stderr, "Working...");
    } else {
        // outFileName not given
        fprintf(stderr, "Warning: output file name missing, won't output anything.\n\n");
    }

    fflush(stderr);
}

float KitchenSync::getBpm() {
    float bpm;
    int nChannels;
    SAMPLETYPE *sampleBuffer = new SAMPLETYPE[BUFF_SIZE];
    std::fill_n(sampleBuffer, BUFF_SIZE, (SAMPLETYPE) 0);

    BPMDetect *bpmDetector = new BPMDetect(inFile->getNumChannels(), inFile->getSampleRate());

    fprintf(stderr, "Detecting BPM rate...\n");

    nChannels = inFile->getNumChannels();
    assert(BUFF_SIZE % nChannels == 0);

    while (inFile->eof() == 0) {
        int num, samples;

        num = inFile->read(sampleBuffer, BUFF_SIZE);
        samples = num / nChannels;

        bpmDetector->inputSamples(sampleBuffer, samples);
    }

    bpm = bpmDetector->getBpm();
    inFile->rewind();

    fprintf(stderr, "Done!\n");

    if (bpm > 0.0) {
        printf("Detected BPM rate %.1f\n\n", bpm);
    } else {
        printf("Couldn't detect BPM rate.\n\n");
    }

    delete bpmDetector;

    return bpm;
}

void KitchenSync::process() {
    int nSamples;
    int nChannels;
    int buffSizeSamples;
    SAMPLETYPE sampleBuffer[BUFF_SIZE];

    assert(inFile != nullptr);
    assert(outFile != nullptr);

    nChannels = inFile->getNumChannels();
    assert(nChannels > 0);
    buffSizeSamples = BUFF_SIZE / nChannels;

    while (inFile->eof() == 0) {
        int num;

        num = inFile->read(sampleBuffer, BUFF_SIZE);
        nSamples = num / (int) inFile->getNumChannels();

        soundTouch->putSamples(sampleBuffer, nSamples);

        // Read ready samples from SoundTouch processor & write them output file.
        // NOTES:
        // - 'receiveSamples' doesn't necessarily return any samples at all
        //   during some rounds!
        // - On the other hand, during some round 'receiveSamples' may have more
        //   ready samples than would fit into 'sampleBuffer', and for this reason
        //   the 'receiveSamples' call is iterated for as many times as it
        //   outputs samples.
        do
        {
            nSamples = soundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
            outFile->write(sampleBuffer, nSamples * nChannels);
        } while (nSamples != 0);
    }

    // Now the input file is processed, yet 'flush' few last samples that are
    // hiding in the SoundTouch's internal processing pipeline.
    soundTouch->flush();
    do {
        nSamples = soundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
        outFile->write(sampleBuffer, nSamples * nChannels);
    } while (nSamples != 0);
}

void KitchenSync::setBpm(float fromBpm, float toBpm) {
    int sampleRate;
    int channels;

    sampleRate = (int) inFile->getSampleRate();
    channels = (int) inFile->getNumChannels();

    soundTouch->setSampleRate(sampleRate);
    soundTouch->setChannels(channels);

    soundTouch->setTempoChange(params->tempoDelta);
    soundTouch->setPitchSemiTones(params->pitchDelta);
    soundTouch->setRateChange(params->rateDelta);

    soundTouch->setSetting(SETTING_USE_QUICKSEEK, params->quick);
    soundTouch->setSetting(SETTING_USE_AA_FILTER, !(params->noAntiAlias));

    if (params->speech) {
        soundTouch->setSetting(SETTING_SEQUENCE_MS, 40);
        soundTouch->setSetting(SETTING_SEEKWINDOW_MS, 15);
        soundTouch->setSetting(SETTING_OVERLAP_MS, 8);
    }

    params->rateDelta = (toBpm / fromBpm - 1.0f) * 100.0f;

    printInfo();

    // clock_t cs = clock();    // for benchmarking processing duration
    // Process the sound
    process();
    // clock_t ce = clock();    // for benchmarking processing duration
    // printf("duration: %lf\n", (double)(ce-cs)/CLOCKS_PER_SEC);

    fprintf(stderr, "Done!\n");
}

float KitchenSync::getQuality() {
    KitchenSyncAnalyser *analyser = new KitchenSyncAnalyser(params->inFileName);

    analyser->printInfo();
    float quality = analyser->analyse();
    analyser->printMagnitudes();

    delete analyser;
    return quality;
}
