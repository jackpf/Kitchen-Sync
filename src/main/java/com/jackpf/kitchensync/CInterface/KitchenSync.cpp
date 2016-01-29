#include "KitchenSync.h"

namespace KitchenSync {
    const char *getVersion() {
        return soundTouch.getVersionString();
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
        }
        else {
            printf("Couldn't detect BPM rate.\n\n");
        }

        return bpmValue;
    }

    void setBpm(const char *inFilename, const char *outFilename, float fromBpm, float toBpm) {
        WavInFile inFile(inFilename);
        WavOutFile outFile(outFilename, (int) inFile.getSampleRate(), (int) inFile.getNumBits(), (int) inFile.getNumChannels());
        float tempoDelta = (toBpm / fromBpm - 1.0f) * 100.0f;

        assert(inFile.getNumChannels() > 0);

        soundTouch.setSampleRate((int) inFile.getSampleRate());
        soundTouch.setChannels((int) inFile.getNumChannels());

        soundTouch.setTempoChange(tempoDelta);
        soundTouch.setPitchSemiTones(0.0);
        soundTouch.setRateChange(0.0);

        soundTouch.setSetting(SETTING_USE_QUICKSEEK, false);
        soundTouch.setSetting(SETTING_USE_AA_FILTER, true);

        // print processing information
    #ifdef SOUNDTOUCH_INTEGER_SAMPLES
        printf("Uses 16bit integer sample type in processing.\n\n");
    #else
        #ifndef SOUNDTOUCH_FLOAT_SAMPLES
            #error "Sampletype not defined"
        #endif
        printf("Uses 32bit floating point sample type in processing.\n\n");
    #endif
        // print processing information only if outFileName given i.e. some processing will happen
        printf("Processing the file with the following changes:\n");
        printf("  tempo change = %+g %%\n", tempoDelta);
        printf("  pitch change = %+g semitones\n", 0.0);
        printf("  rate change  = %+g %%\n\n", 0.0);
        printf("Working...\n");

        // Process
        int nSamples;
        int nChannels = (int) inFile.getNumChannels();
        assert(nChannels > 0);
        int buffSizeSamples = BUFF_SIZE / nChannels;
        SAMPLETYPE sampleBuffer[BUFF_SIZE];

        // Process samples read from the input file
        while (inFile.eof() == 0) {
            // Read a chunk of samples from the input file
            int num = inFile.read(sampleBuffer, BUFF_SIZE);
            nSamples = num / (int) inFile.getNumChannels();

            // Feed the samples into SoundTouch processor
            soundTouch.putSamples(sampleBuffer, nSamples);

            // Read ready samples from SoundTouch processor & write them output file.
            // NOTES:
            // - 'receiveSamples' doesn't necessarily return any samples at all
            //   during some rounds!
            // - On the other hand, during some round 'receiveSamples' may have more
            //   ready samples than would fit into 'sampleBuffer', and for this reason
            //   the 'receiveSamples' call is iterated for as many times as it
            //   outputs samples.
            do {
                nSamples = soundTouch.receiveSamples(sampleBuffer, buffSizeSamples);
                outFile.write(sampleBuffer, nSamples * nChannels);
            } while (nSamples != 0);
        }

        // Now the input file is processed, yet 'flush' few last samples that are
        // hiding in the SoundTouch's internal processing pipeline.
        soundTouch.flush();
        do {
            nSamples = soundTouch.receiveSamples(sampleBuffer, buffSizeSamples);
            outFile.write(sampleBuffer, nSamples * nChannels);
        } while (nSamples != 0);

        printf("Done!\n");
    }
}
