#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "dynamicTau/actorDynTau.hpp"
#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/environment/classicEnv.hpp"
#include "dynamicTau/classicDynTau.hpp"

int main() {
    // Create a vector of input values for the decision agent
    ClassicEnvDynTau env;

    // env.reset("newParams.txt");
    env.reset();

    Feature SNRinit = env.bf.windowAverage(env.bf.SNR, env.state().size());

    classicDynTau<Feature> algo(SNRinit, env.bf.targetCoupling);

    std::vector<int> stepInfo;
    int scanCount=0;

    while (true){
        int choice;

        #ifdef MANUAL
            std::cout << "Enter 1 to take more data, 2 to step forward or 0 to exit: ";
            std::cin >> choice;
            choice --;
        #else
            choice = algo.proposeAction(env.state());
            // choice = (scanCount >= 4);
        #endif

        if(choice == -1 || env.done()){
            break;
        }
        else {
            env.applyAction(choice);
            if (choice){
                std::cout << "Stepping forward." << std::endl;
                stepInfo.push_back(scanCount);

                scanCount = 0;
            }
            else{
                scanCount++;
            }
        }
        
        // std::cout << "Current score: " << algo.checkScore(env.state()) << " vs. threshold " << algo.threshold << std::endl;
        Feature state = env.state();
        Feature stateAxis = env.stateAxis();

        std::vector<double> vecState(state.begin(), state.end());
        std::vector<double> vecStateAxis(stateAxis.begin(), stateAxis.end());

        std::vector<double> activeWindow(env.bf.exclusionLine.begin()+env.bf.startIndex, env.bf.exclusionLine.end());
        std::vector<double> activeAxis(env.bf.fullFreqRange.begin()+env.bf.startIndex, env.bf.fullFreqRange.end());

        std::vector<double> SNR = env.bf.SNR;
        double rescale = std::max_element(vecState.begin(), vecState.end())[0];

        for (int i = 0; i < SNR.size(); i++) SNR[i] *= rescale;

        plt::clf();

        plt::plot(activeAxis, activeWindow);
        plt::plot(activeAxis, std::vector<double> (activeAxis.size(), env.bf.targetCoupling));
        plt::plot(activeAxis, SNR);
        plt::scatter(vecStateAxis, vecState, 20, {{"color", "red"}});

        plt::pause(0.001);
    }

    std::cout << "Total scans requested: " << std::accumulate(stepInfo.begin(), stepInfo.end(), 0) << std::endl;
    std::cout << "Average scans requested: " << (double)std::accumulate(stepInfo.begin(), stepInfo.end(), 0)/(double)stepInfo.size() << std::endl;
    std::cout << "Total steps taken: " << stepInfo.size() << std::endl;

    plt::show();

    plt::clf();

    std::vector<double> fullWindow(env.bf.exclusionLine.begin()+env.bf.rewardStartIndex, env.bf.exclusionLine.begin()+env.bf.rewardEndIndex);
    std::vector<double> fullAxis(env.bf.fullFreqRange.begin()+env.bf.rewardStartIndex, env.bf.fullFreqRange.begin()+env.bf.rewardEndIndex);

    plt::plot(fullAxis, fullWindow);
    plt::plot(fullAxis, std::vector<double> (fullAxis.size(), env.bf.targetCoupling));
    plt::show();

    return 0;
}