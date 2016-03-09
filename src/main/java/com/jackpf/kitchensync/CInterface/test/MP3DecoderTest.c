#include <stdio.h>
#include <iostream>
#include <lame/lame.h>

#include "../include/MP3Decoder.h"
#include "../include/WavFile.h"

using namespace std;

int main(int argc, char *argv[])
{
    MP3Decoder d(argv[1]);
    WavOutFile outFile(argv[2], 44100, 16, 2);
    float buf[1024];

    while (d.eof() == 0) {
        int read = d.read(buf, 1024);
        outFile.write(buf, read);
    }

    return 0;
}