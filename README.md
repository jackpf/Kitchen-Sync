# Kitchen-Sync

Kitchen Sync aims to be a swiss army knife for preparing audio files for DJ use.

It can calculate the BPM of audio files, allow editing for inaccuracies, then sync all files to a target BPM.

It also analyses the quality of audio files, and can suggest when a tune may have been compressed, then resampled to 320kbps which would be otherwise undetectable (apart from the audible quality, if your ears are sharp enough!)

Eventually it will feature a burn direct to disk feature, making Kitchen Sync the all-in-one DJ preparation tool.

Currently being developed for OS X, but will eventually be tested cross-platform.

## Build Instructions
Java project can be built with Maven.

The project requires a shared library to be built, this can be compiled and installed using ./src/main/java/compile_lib_source.sh
In order to build, you will need the soundtouch and fftw3 libraries installed on your system.

Cheers!
