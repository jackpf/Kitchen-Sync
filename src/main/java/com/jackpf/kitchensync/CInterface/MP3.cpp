#include <iostream>

#include <fftw3.h>
#include <soundtouch/SoundTouch.h>
#include "include/WavFile.h"

using namespace std;
using namespace soundtouch;

#define BUFF_SIZE           160000

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Incorrect args" << endl;
        return -1;
    }

    SAMPLETYPE sampleBuffer[BUFF_SIZE];
    WavInFile inFile(argv[1]);

    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFF_SIZE);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFF_SIZE);

    size_t samplesRead = inFile.read(sampleBuffer, BUFF_SIZE);

    cout << "Read " << samplesRead << " samples" << endl;

    //...
    p = fftw_plan_dft_1d(BUFF_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < BUFF_SIZE; i++) {
        in[i][0] = sampleBuffer[i];
        //cout << "in[" << i << "][0] = " << sampleBuffer[i] << endl;
    }

    //...
    fftw_execute(p); /* repeat as needed */
    //...
    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);

    cout << "Done" << endl;

    for (int i = 0; i < BUFF_SIZE; i++) {
        cout << i << ": " << out[i][0] << ", " << out[i][1] << endl;
    }
 }