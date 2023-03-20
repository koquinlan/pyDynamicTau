#include "dynamicTau/classic/scanRunner.hpp"

int main() {
    ScanRunner runner;

    runner.runScan(0);
    runner.showFinal();
    runner.saveFinal();

    return 0;
}