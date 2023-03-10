#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <iostream>
#include <algorithm>

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

    classicDynTau<Feature> algo(SNRinit);

    while(true){
        int choice;

        #ifdef MANUAL
            std::cout << "Enter 1 to take more data, 2 to step forward or 0 to exit: ";
            std::cin >> choice;
            choice --;
        #else
            choice = algo.proposeAction(env.state());
        #endif

        if(choice == -1 || env.done()){
            break;
        }
        else {
            env.applyAction(choice);
            std::cout << choice << std::endl;
        }
        
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

        plt::show();

        plt::pause(0.01);
    }

    std::cout << "Final reward: " << env.reward() << std::endl;

    std::cout << "Reward window between " << env.bf.fullFreqRange[env.bf.rewardStartIndex] << " and " 
    << env.bf.fullFreqRange[env.bf.rewardEndIndex] << " MHz" << std::endl;

    Feature state = env.state();
    std::vector<double> vecState(state.begin(), state.end());

    plt::plot(vecState);
    plt::show();


    plt::show();

    return 0;
}