#include "AudioFile.h"
#include "WavFile.h"
#include "FLACDecoder.h"
#include "AIFFDecoder.h"
#include "MP3Decoder.h"

AudioInFile *AudioFileFactory::createAudioInFile(const char *cFilename) {
    std::string filename(cFilename);
    std::string ext = filename.substr(filename.find_last_of(".") + 1);

    if (ext == "wav") {
        return new WavInFile(cFilename);
    } else if (ext == "flac") {
        return new FLACDecoder(cFilename);
    } else if (ext == "aiff") {
        return new AIFFDecoder(cFilename);
    } else if (ext == "mp3") {
        return new MP3Decoder(cFilename);
    } else {
        throw std::runtime_error(std::string("No decoder found for extension: ") + ext);
    }
}