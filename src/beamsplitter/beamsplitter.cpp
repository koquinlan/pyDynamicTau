#include "beamsplitter/environment/EnvironmentBeamsplitter.hpp"
#include "beamsplitter/ActorBeamsplitter.hpp"
#include "learner/reinforcement_learner/ReinforcementLearner.hpp"

constexpr int numTrialDefault = 100;
constexpr int numEpisodeDefault = 500;

int main()
{
  ReinforcementLearner<ActorBeamsplitter, Feature, EnvironmentBeamsplitter> learner{numTrialDefault, numEpisodeDefault};
  learner.run();
  learner.printResults();
}
