#ifndef __Kitchen_Sync_Audio_File_h__
#define __Kitchen_Sync_Audio_File_h__

#include <sstream>

class AudioInFile {
public:
    virtual ~AudioInFile() { };

    virtual int eof() const = 0;
    virtual int read(float *buf, int len) = 0;
    virtual uint getNumChannels() const = 0;
    virtual uint getSampleRate() const = 0;
    virtual uint getBytesPerSample() const = 0;
    virtual uint getNumSamples() const = 0;
    virtual uint getNumBits() const = 0;
    virtual void rewind() = 0;
};

class AudioFileFactory {
public:
    static AudioInFile *createAudioInFile(const char *cFilename);
};

#endif