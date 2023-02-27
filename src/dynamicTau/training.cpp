#include "dynamicTau/environment/environmentDynTau.hpp"
#include "dynamicTau/actorDynTau.hpp"

#include "learner/reinforcement_learner/ReinforcementLearner.hpp"

constexpr int numTrialDefault = 1;
constexpr int numEpisodeDefault = 5;

int main()
{
  ReinforcementLearner<ActorDynTau, Feature, EnvironmentDynTau> learner{numTrialDefault, numEpisodeDefault};
  learner.run();
  learner.printResults();
}