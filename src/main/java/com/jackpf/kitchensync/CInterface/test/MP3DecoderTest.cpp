#include <stdio.h>
#include <iostream>
#include <lame/lame.h>

#include "../include/MP3Decoder.h"
#include "../include/WavFile.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <inFile.mp3> <outFile.wav>" << endl;
        return -1;
    }

    MP3Decoder d(argv[1]);
    WavOutFile outFile(argv[2], 44100, 16, 2);
    float buf[1024];
    long total = 0;

    while (d.eof() == 0) {
        int read = d.read(buf, 1024);
        outFile.write(buf, read);
        total += read;
    }

    cout << "Finished: wrote " << total << " bytes" << endl;

    return 0;
}