#include "KitchenSync.h"

namespace KitchenSync {
    const char *getVersion() {
        return soundTouch.getVersionString();
    }

    static void openFiles(WavInFile **inFile, WavOutFile **outFile, const RunParameters *params) {
        int bits, samplerate, channels;

        *inFile = new WavInFile(params->inFileName);

        // ... open output file with same sound parameters
        bits = (int) (*inFile)->getNumBits();
        samplerate = (int) (*inFile)->getSampleRate();
        channels = (int) (*inFile)->getNumChannels();

        if (params->outFileName && *outFile != nullptr) {
            *outFile = new WavOutFile(params->outFileName, samplerate, bits, channels);
        } else {
            *outFile = NULL;
        }
    }

    float getBpm(const char *filename) {
        float bpmValue;
        int nChannels;
        WavInFile inFile(filename);
        BPMDetect bpm(inFile.getNumChannels(), inFile.getSampleRate());
        SAMPLETYPE sampleBuffer[BUFF_SIZE];

        // detect bpm rate
        printf("Detecting BPM rate...\n");

        nChannels = (int) inFile.getNumChannels();
        assert(BUFF_SIZE % nChannels == 0);

        // Process the 'inFile' in small blocks, repeat until whole file has
        // been processed
        while (inFile.eof() == 0) {
            int num, samples;

            // Read sample data from input file
            num = inFile.read(sampleBuffer, BUFF_SIZE);

            // Enter the new samples to the bpm analyzer class
            samples = num / nChannels;
            bpm.inputSamples(sampleBuffer, samples);
        }

        // Now the whole song data has been analyzed. Read the resulting bpm.
        bpmValue = bpm.getBpm();
        printf("Done!\n");

        // rewind the file after bpm detection
        inFile.rewind();

        if (bpmValue > 0) {
            printf("Detected BPM rate %.1f\n\n", bpmValue);
        } else {
            printf("Couldn't detect BPM rate.\n\n");
        }

        return bpmValue;
    }

    static void setup(SoundTouch *pSoundTouch, const WavInFile *inFile, const RunParameters *params) {
        int sampleRate;
        int channels;

        sampleRate = (int) inFile->getSampleRate();
        channels = (int) inFile->getNumChannels();
        pSoundTouch->setSampleRate(sampleRate);
        pSoundTouch->setChannels(channels);

        pSoundTouch->setTempoChange(params->tempoDelta);
        pSoundTouch->setPitchSemiTones(params->pitchDelta);
        pSoundTouch->setRateChange(params->rateDelta);

        pSoundTouch->setSetting(SETTING_USE_QUICKSEEK, params->quick);
        pSoundTouch->setSetting(SETTING_USE_AA_FILTER, !(params->noAntiAlias));

        if (params->speech) {
            // use settings for speech processing
            pSoundTouch->setSetting(SETTING_SEQUENCE_MS, 40);
            pSoundTouch->setSetting(SETTING_SEEKWINDOW_MS, 15);
            pSoundTouch->setSetting(SETTING_OVERLAP_MS, 8);
            fprintf(stderr, "Tune processing parameters for speech processing.\n");
        }

        // print processing information
        if (params->outFileName) {
    #ifdef SOUNDTOUCH_INTEGER_SAMPLES
            fprintf(stderr, "Uses 16bit integer sample type in processing.\n\n");
    #else
        #ifndef SOUNDTOUCH_FLOAT_SAMPLES
            #error "Sampletype not defined"
        #endif
            fprintf(stderr, "Uses 32bit floating point sample type in processing.\n\n");
    #endif
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

    static void process(SoundTouch *pSoundTouch, WavInFile *inFile, WavOutFile *outFile) {
        int nSamples;
        int nChannels;
        int buffSizeSamples;
        SAMPLETYPE sampleBuffer[BUFF_SIZE];

        if ((inFile == NULL) || (outFile == NULL)) return;  // nothing to do.

        nChannels = (int) inFile->getNumChannels();
        assert(nChannels > 0);
        buffSizeSamples = BUFF_SIZE / nChannels;

        // Process samples read from the input file
        while (inFile->eof() == 0) {
            int num;

            // Read a chunk of samples from the input file
            num = inFile->read(sampleBuffer, BUFF_SIZE);
            nSamples = num / (int) inFile->getNumChannels();

            // Feed the samples into SoundTouch processor
            pSoundTouch->putSamples(sampleBuffer, nSamples);

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
                nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
                outFile->write(sampleBuffer, nSamples * nChannels);
            } while (nSamples != 0);
        }

        // Now the input file is processed, yet 'flush' few last samples that are
        // hiding in the SoundTouch's internal processing pipeline.
        pSoundTouch->flush();
        do {
            nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
            outFile->write(sampleBuffer, nSamples * nChannels);
        } while (nSamples != 0);
    }

    void setBpm(const char *inFilename, const char *outFilename, float fromBpm, float toBpm) {
        WavInFile *inFile;
        WavOutFile *outFile;
        RunParameters *params;
        SoundTouch soundTouch;

        try
        {
            // Parse command line parameters
            params = new RunParameters();

            params->inFileName = inFilename;
            params->outFileName = outFilename;
            params->rateDelta = (toBpm / fromBpm - 1.0f) * 100.0f;

            // Open input & output files
            openFiles(&inFile, &outFile, params);

            // Setup the 'SoundTouch' object for processing the sound
            setup(&soundTouch, inFile, params);

            // clock_t cs = clock();    // for benchmarking processing duration
            // Process the sound
            process(&soundTouch, inFile, outFile);
            // clock_t ce = clock();    // for benchmarking processing duration
            // printf("duration: %lf\n", (double)(ce-cs)/CLOCKS_PER_SEC);

            // Close WAV file handles & dispose of the objects
            delete inFile;
            delete outFile;
            delete params;

            fprintf(stderr, "Done!\n");
        }
        catch (const std::runtime_error &e)
        {
            // An exception occurred during processing, display an error message
            fprintf(stderr, "%s\n", e.what());
        }
    }

    float getQuality(const char *filename) {
        return analyse(filename);
    }
}
