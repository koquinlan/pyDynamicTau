#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <iostream>

#include "dynamicTau/environment/environmentDynTau.hpp"
#include "dynamicTau/actorDynTau.hpp"


// Define the input size and output size of the RL agent
const int INPUT_SIZE = 14;
const int OUTPUT_SIZE = 2;

int main() {
    ActorDynTau actor;

    actor.load("param");

    // Create a vector of input values for the RL agent
    EnvironmentDynTau env;
    env.reset();
    env.applyAction(0);

    while(!env.done()){
        // Use the RL agent to choose an action based on the input tensor
        Feature previousState = env.state();
        int action = actor.proposeAction(previousState, 0);

        env.applyAction(action);

        std::cout << "Applying action: " << action << std::endl;
    }

    Feature state = env.state();
    std::vector<double> vecState(state.begin(), state.end());

    plt::plot(vecState);

    // plt::plot(env.bf.fullFreqRange, env.bf.exclusionLine);

    plt::show();

    return 0;
}
