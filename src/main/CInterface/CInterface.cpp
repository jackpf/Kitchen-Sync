#include <jni.h>
#include <stdio.h>

#include <soundtouch/SoundTouch.h>
#include <soundtouch/BPMDetect.h>

#include "CInterface.h"
#include "WavFile.h"

using namespace soundtouch;

// Processing chunk size (size chosen to be divisible by 2, 4, 6, 8, 10, 12, 14, 16 channels ...)
#define BUFF_SIZE           6720

SoundTouch soundTouch;

JNIEXPORT jstring JNICALL Java_CInterface_getVersion
  (JNIEnv *env, jobject obj)
{
    return env->NewStringUTF(soundTouch.getVersionString());
}

JNIEXPORT jstring JNICALL Java_CInterface_getBpm
  (JNIEnv *env, jobject obj, jstring jFilename)
{
    float bpmValue, goalBPM = 130.0, tempoDelta;
    int nChannels;
    const char *filename = env->GetStringUTFChars(jFilename, 0);
    WavInFile *inFile = new WavInFile(filename);
    env->ReleaseStringUTFChars(jFilename, filename);
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
        return nullptr;
    }

    if (goalBPM > 0)
    {
        // adjust tempo to given bpm
        tempoDelta = (goalBPM / bpmValue - 1.0f) * 100.0f;
        fprintf(stderr, "The file will be converted to %.1f BPM\n\n", goalBPM);
    }

    char buf[64];
    snprintf(buf, 64, "%.1f", bpmValue);

    return env->NewStringUTF(buf);
}