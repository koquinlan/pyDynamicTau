#include "dynamicTau/classic/scanRunner.hpp"

int main() {
    ScanRunner runner;

    runner.runScan(3);
    runner.showFinal();
    runner.saveFinal();

    return 0;
}