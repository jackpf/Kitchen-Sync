#include "AudioFile.h"
#include "WavFile.h"
#include "FLACDecoder.h"

AudioInFile *AudioFileFactory::createAudioInFile(const char *cFilename) {
    std::string filename(cFilename);
    std::string ext = filename.substr(filename.find_last_of(".") + 1);

    if (ext == "wav") {
        return new WavInFile(cFilename);
    } else if (ext == "flac") {
        return new FLACDecoder(cFilename);
    } else {
        return nullptr;
    }
}