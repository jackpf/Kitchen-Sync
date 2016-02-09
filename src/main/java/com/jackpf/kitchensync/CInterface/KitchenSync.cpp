#include "KitchenSync.h"
#include "RunParameters.h"
#include <sstream>
#if _WIN32
    #include <io.h>
    #include <fcntl.h>

    // Macro for Win32 standard input/output stream support: Sets a file stream into binary mode
    #define SET_STREAM_TO_BIN_MODE(f) (_setmode(_fileno(f), _O_BINARY))
#else
    // Not needed for GNU environment...
    #define SET_STREAM_TO_BIN_MODE(f) {}
#endif

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
        } else {
            printf("Couldn't detect BPM rate.\n\n");
        }

        return bpmValue;
    }

    static void openFiles(WavInFile **inFile, WavOutFile **outFile, const RunParameters *params)
    {
        int bits, samplerate, channels;

        if (strcmp(params->inFileName, "stdin") == 0)
        {
            // used 'stdin' as input file
            SET_STREAM_TO_BIN_MODE(stdin);
            *inFile = new WavInFile(stdin);
        }
        else
        {
            // open input file...
            *inFile = new WavInFile(params->inFileName);
        }

        // ... open output file with same sound parameters
        bits = (int)(*inFile)->getNumBits();
        samplerate = (int)(*inFile)->getSampleRate();
        channels = (int)(*inFile)->getNumChannels();

        if (params->outFileName)
        {
            if (strcmp(params->outFileName, "stdout") == 0)
            {
                SET_STREAM_TO_BIN_MODE(stdout);
                *outFile = new WavOutFile(stdout, samplerate, bits, channels);
            }
            else
            {
                *outFile = new WavOutFile(params->outFileName, samplerate, bits, channels);
            }
        }
        else
        {
            *outFile = NULL;
        }
    }



    // Sets the 'SoundTouch' object up according to input file sound format &
    // command line parameters
    static void setup(SoundTouch *pSoundTouch, const WavInFile *inFile, const RunParameters *params)
    {
        int sampleRate;
        int channels;

        sampleRate = (int)inFile->getSampleRate();
        channels = (int)inFile->getNumChannels();
        pSoundTouch->setSampleRate(sampleRate);
        pSoundTouch->setChannels(channels);

        pSoundTouch->setTempoChange(params->tempoDelta);
        pSoundTouch->setPitchSemiTones(params->pitchDelta);
        pSoundTouch->setRateChange(params->rateDelta);

        pSoundTouch->setSetting(SETTING_USE_QUICKSEEK, params->quick);
        pSoundTouch->setSetting(SETTING_USE_AA_FILTER, !(params->noAntiAlias));

        if (params->speech)
        {
            // use settings for speech processing
            pSoundTouch->setSetting(SETTING_SEQUENCE_MS, 40);
            pSoundTouch->setSetting(SETTING_SEEKWINDOW_MS, 15);
            pSoundTouch->setSetting(SETTING_OVERLAP_MS, 8);
            fprintf(stderr, "Tune processing parameters for speech processing.\n");
        }

        // print processing information
        if (params->outFileName)
        {
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
        }
        else
        {
            // outFileName not given
            fprintf(stderr, "Warning: output file name missing, won't output anything.\n\n");
        }

        fflush(stderr);
    }



    // Processes the sound
    static void process(SoundTouch *pSoundTouch, WavInFile *inFile, WavOutFile *outFile)
    {
        int nSamples;
        int nChannels;
        int buffSizeSamples;
        SAMPLETYPE sampleBuffer[BUFF_SIZE];

        if ((inFile == NULL) || (outFile == NULL)) return;  // nothing to do.

        nChannels = (int)inFile->getNumChannels();
        assert(nChannels > 0);
        buffSizeSamples = BUFF_SIZE / nChannels;

        // Process samples read from the input file
        while (inFile->eof() == 0)
        {
            int num;

            // Read a chunk of samples from the input file
            num = inFile->read(sampleBuffer, BUFF_SIZE);
            nSamples = num / (int)inFile->getNumChannels();

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
        do
        {
            nSamples = pSoundTouch->receiveSamples(sampleBuffer, buffSizeSamples);
            outFile->write(sampleBuffer, nSamples * nChannels);
        } while (nSamples != 0);
    }



    // Detect BPM rate of inFile and adjust tempo setting accordingly if necessary
    static void detectBPM(WavInFile *inFile, RunParameters *params)
    {
        float bpmValue;
        int nChannels;
        BPMDetect bpm(inFile->getNumChannels(), inFile->getSampleRate());
        SAMPLETYPE sampleBuffer[BUFF_SIZE];

        // detect bpm rate
        fprintf(stderr, "Detecting BPM rate...");
        fflush(stderr);

        nChannels = (int)inFile->getNumChannels();
        assert(BUFF_SIZE % nChannels == 0);

        // Process the 'inFile' in small blocks, repeat until whole file has
        // been processed
        while (inFile->eof() == 0)
        {
            int num, samples;

            // Read sample data from input file
            num = inFile->read(sampleBuffer, BUFF_SIZE);

            // Enter the new samples to the bpm analyzer class
            samples = num / nChannels;
            bpm.inputSamples(sampleBuffer, samples);
        }

        // Now the whole song data has been analyzed. Read the resulting bpm.
        bpmValue = bpm.getBpm();
        fprintf(stderr, "Done!\n");

        // rewind the file after bpm detection
        inFile->rewind();

        if (bpmValue > 0)
        {
            fprintf(stderr, "Detected BPM rate %.1f\n\n", bpmValue);
        }
        else
        {
            fprintf(stderr, "Couldn't detect BPM rate.\n\n");
            return;
        }

        if (params->goalBPM > 0)
        {
            // adjust tempo to given bpm
            params->tempoDelta = (params->goalBPM / bpmValue - 1.0f) * 100.0f;
            fprintf(stderr, "The file will be converted to %.1f BPM\n\n", params->goalBPM);
        }
    }

    void setBpm(const char *inFilename, const char *outFilename, float fromBpm, float toBpm) {
        WavInFile *inFile;
        WavOutFile *outFile;
        RunParameters *params;
        SoundTouch soundTouch;

        float tempoDelta = (toBpm / fromBpm - 1.0f) * 100.0f;
        std::ostringstream stringStream;
        stringStream << "-rate=" << (tempoDelta >= 0 ? "+" : "") << std::to_string(tempoDelta);

        const char *paramStr[] = {"", inFilename, outFilename, stringStream.str().c_str()};

        try
        {
            // Parse command line parameters
            params = new RunParameters(4, paramStr);

            // Open input & output files
            openFiles(&inFile, &outFile, params);

            if (params->detectBPM == true)
            {
                // detect sound BPM (and adjust processing parameters
                //  accordingly if necessary)
                detectBPM(inFile, params);
            }

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
}
