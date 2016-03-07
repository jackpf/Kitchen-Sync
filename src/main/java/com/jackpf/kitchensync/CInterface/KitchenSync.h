#include <stdio.h>
#include <sstream>

#include <soundtouch/SoundTouch.h>
#include <soundtouch/BPMDetect.h>

#include "WavFile.h"
#include "FLACDecoder.h"
#include "AudioFile.h"
#include "KitchenSyncAnalyser.h"

using namespace soundtouch;

typedef struct {
    const char  *inFileName     = nullptr;
    const char  *outFileName    = nullptr;
    float       tempoDelta      = 0.0;
    float       pitchDelta      = 0.0;
    float       rateDelta       = 0.0;
    int         quick           = 0;
    int         noAntiAlias     = 0;
    float       goalBPM         = 0.0;
    //bool        detectBPM;
    bool        speech          = false;
} RunParameters;

class KitchenSync {
private:
    const int BUFF_SIZE;

    SoundTouch *soundTouch;
    RunParameters *params;

    AudioInFile *inFile = nullptr;
    WavOutFile *outFile = nullptr;

    void openFiles();
    void process();

public:
    KitchenSync(RunParameters *params);
    ~KitchenSync();

    const char *getVersion();
    void printInfo();
    float getBpm();
    void setBpm(float fromBpm, float toBpm);
    float getQuality();
};