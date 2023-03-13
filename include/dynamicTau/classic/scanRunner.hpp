#pragma once

#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/classic/classicEnv.hpp"
#include "dynamicTau/classic/classicDynTau.hpp"

class ScanRunner {

public:

    ClassicEnvDynTau env;
    classicDynTau<Feature> algo;

    std::vector<int> stepInfo;
    int scanCount=0;

    ScanRunner() : env() {
        env.reset();

        Feature SNRinit = env.bf.windowAverage(env.bf.SNR, env.state().size());
        algo = classicDynTau<Feature>(SNRinit, env.bf.targetCoupling);
    }

    void runScan(int plotting = 2);

    int makeChoice();
    int queryChoice();
    int applyChoice(int choice);

    void showState(int continuousPlotting = 1);
    void showFinal();
};