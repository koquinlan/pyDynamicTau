#include "dynamicTau/environment/environmentDynTau.hpp"
#include "dynamicTau/actorDynTau.hpp"

#include "learner/reinforcement_learner/ReinforcementLearner.hpp"

constexpr int numTrialDefault = 30;
constexpr int numEpisodeDefault = 100;

int main()
{
  ReinforcementLearner<ActorDynTau, Feature, EnvironmentDynTau> learner{numTrialDefault, numEpisodeDefault};
  learner.run();
}