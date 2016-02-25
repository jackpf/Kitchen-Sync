#include "KitchenSync.h"

namespace KitchenSync {
    const char *getVersion() {
        return soundTouch.getVersionString();
    }

    static void openFiles(WavInFile **inFile, WavOutFile **outFile, const RunParameters *params) {
        int bits, samplerate, channels;

        *inFile = new WavInFile(params->inFileName);

        bits = (int) (*inFile)->getNumBits();
        samplerate = (int) (*inFile)->getSampleRate();
        channels = (int) (*inFile)->getNumChannels();

        if (*outFile != nullptr) {
            *outFile = new WavOutFile(params->outFileName, samplerate, bits, channels);
        }
    }

    static void printInfo(const RunParameters *params) {
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

    float getBpm(const char *filename) {
        float bpm;
        int nChannels;
        
        RunParameters *params;
        WavInFile *inFile;
        BPMDetect *bpmDetector;
        SAMPLETYPE sampleBuffer[BUFF_SIZE];

        params = new RunParameters();
        params->inFileName = filename;
        openFiles(&inFile, nullptr, params);
        bpmDetector = new BPMDetect(inFile->getNumChannels(), inFile->getSampleRate());

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

        delete params;
        delete inFile;
        delete bpmDetector;

        return bpm;
    }

    static void setup(SoundTouch *soundTouch, const WavInFile *inFile, const RunParameters *params) {
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

        printInfo(params);
    }

    static void process(SoundTouch *soundTouch, WavInFile *inFile, WavOutFile *outFile) {
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

    void setBpm(const char *inFilename, const char *outFilename, float fromBpm, float toBpm) {
        WavInFile *inFile;
        WavOutFile *outFile;
        RunParameters *params;
        SoundTouch *soundTouch;

        soundTouch = new SoundTouch();
        params = new RunParameters();

        params->inFileName = inFilename;
        params->outFileName = outFilename;
        params->rateDelta = (toBpm / fromBpm - 1.0f) * 100.0f;

        openFiles(&inFile, &outFile, params);

        setup(soundTouch, inFile, params);

        // clock_t cs = clock();    // for benchmarking processing duration
        // Process the sound
        process(soundTouch, inFile, outFile);
        // clock_t ce = clock();    // for benchmarking processing duration
        // printf("duration: %lf\n", (double)(ce-cs)/CLOCKS_PER_SEC);

        delete inFile;
        delete outFile;
        delete params;
        delete soundTouch;

        fprintf(stderr, "Done!\n");
    }

    float getQuality(const char *filename) {
        KitchenSyncAnalyser *analyser = new KitchenSyncAnalyser(filename);
        analyser->printInfo();
        float quality = analyser->analyse();
        analyser->printMagnitudes();

        delete analyser;
        return quality;
    }
}
