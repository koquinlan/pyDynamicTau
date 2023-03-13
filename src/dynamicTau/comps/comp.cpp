#include "dynamicTau/classic/scanRunner.hpp"

int main() {
    ScanRunner runner;

    runner.runScan();
    runner.showFinal();
    runner.saveFinal();

    return 0;
}