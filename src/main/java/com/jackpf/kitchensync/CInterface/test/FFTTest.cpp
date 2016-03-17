#include <iostream>
#include "../include/KitchenSyncAnalyser.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <inFile>" << endl;
    }

    KitchenSyncAnalyser *analyser = new KitchenSyncAnalyser(argv[1]);

    analyser->printInfo();
    float quality = analyser->analyse();
    analyser->printMagnitudes();

    delete analyser;

    return 0;
}