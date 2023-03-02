#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <iostream>

#include "dynamicTau/actorDynTau.hpp"
#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/environment/environmentDynTau.hpp"


int main() {
    ActorDynTau actor;

    actor.load("param");

    // Create a vector of input values for the RL agent
    EnvironmentDynTau env;
    env.reset();

    // Modify some of the scanning parameters
    env.bf.scanningWindowSize = 10;
    env.bf.targetCoupling = 0.02;

    env.applyAction(0);

    while(!env.done()){
        std::cout << "Score: " << env.score()<< std::endl;

        // Use the RL agent to choose an action based on the input tensor
        Feature previousState = env.state();
        int action = actor.proposeAction(previousState, 0);

        env.applyAction(action);

        
        std::cout << "Applying action: " << action << std::endl;
    }

    std::cout << "Final reward: " << env.reward() << std::endl;

    Feature state = env.state();
    std::vector<double> vecState(state.begin(), state.end());

    plt::plot(vecState);
    plt::show();

    plt::plot(env.bf.fullFreqRange, env.bf.exclusionLine);
    plt::plot(env.bf.fullFreqRange, std::vector<double> (env.bf.fullFreqRange.size(), env.bf.targetCoupling));
    plt::show();

    return 0;
}
