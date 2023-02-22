#include <iostream>
#include <fstream>

#include "actor.hpp"
#include "environment.hpp"
#include "util.hpp"

constexpr int numTrial = 100;
constexpr int numEpisode = 500;

int main() {
    // Initialize environment and actor
    Environment environment;
    Actor<Feature, environment.numAction> actor;

    //  actor.load("param");
    std::vector<int> saveAction, currentAction;
    double maxReward = 0;

    for (int trial = 0; trial < numTrial; trial++) {
        // Liklihood of random action decays as training proceeds
        double epsilon = epsilonByProportion(trial / (double)numTrial);

        double avgLoss = 0;
        double maxRewardTrial = 0;
        int lossCtr = 0;

        for (int episode = 0; episode < numEpisode; episode++) {
            currentAction.clear();
            environment.reset();

            Feature state = environment.state();
            // Check completion
            bool done = environment.done();

            while (!done) {
                // Select an action using epislon-greedy protocol
                int action = actor.proposeAction(state, epsilon);

                // Add proposed action to currentAction vector and apply it
                currentAction.push_back(action);
                environment.applyAction(action);

                // Calculate the reward after the action was performed
                double reward = environment.reward();

                // Get the updated state (s`)and check for completion
                Feature next_state = environment.state();
                done = environment.done();

                // Push the new information to the actor
                actor.push(state, action, next_state, reward, done);

                // Find loss: squared diff btwn Q(s,a) and Y(s`) 
                avgLoss += actor.learn();
                lossCtr++;

                // Prepare for next episode
                state = next_state;
            }

            // Check if this episode ended with the highest reward so far in the trial
            maxRewardTrial = std::max(maxRewardTrial, environment.reward());

            // If maximum overall reward was achieved on this trial, save the actions that produced it
            if (maxReward < environment.reward()) {
                maxReward = environment.reward();
                saveAction = currentAction;
            }
        }

        // Output information about the trial
        std::cout << trial << "\tEpsilon: " << epsilon 
            << "\tAvgloss: " << avgLoss / lossCtr 
            << "\tmaxRewardTrial: " << maxRewardTrial
            << "\tmaxReward: " << maxReward 
            << std::endl;
    }

    // Save the trained actor parameters
    actor.save("param");

    exit(0);
}