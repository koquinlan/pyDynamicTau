#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <iostream>

#include "dynamicTau/actorDynTau.hpp"
#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/environment/classicEnv.hpp"



int main() {
    // Create a vector of input values for the decision agent
    ClassicEnvDynTau env;

    // env.reset("newParams.txt");
    env.reset();

    while(true){
        std::cout << "Enter 1 to take more data, 2 to step forward or 0 to exit: ";
        int choice;
        std::cin >> choice;

        if(choice == 0){
            break;
        }
        else {
            env.applyAction((int) (choice-1));
        }

        Feature state = env.state();
        Feature stateAxis = env.stateAxis();

        std::vector<double> vecState(state.begin(), state.end());
        std::vector<double> vecStateAxis(stateAxis.begin(), stateAxis.end());

        std::vector<double> activeWindow(env.bf.exclusionLine.begin()+env.bf.startIndex, env.bf.exclusionLine.end());
        std::vector<double> activeAxis(env.bf.fullFreqRange.begin()+env.bf.startIndex, env.bf.fullFreqRange.end());

        plt::plot(activeAxis, activeWindow);
        plt::plot(activeAxis, std::vector<double> (activeAxis.size(), env.bf.targetCoupling));
        plt::scatter(vecStateAxis, vecState);

        plt::show();
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