#pragma once

#include "../trial_runner/TrialResult.hpp"
#include "EpisodeResult.hpp"

struct Act {
    int action;
    double loss;
};

template<class actorClass, class stateClass, class environmentClass>
class EpisodeRunner {
public:
    explicit EpisodeRunner(double chanceOfRandomChoice, actorClass& actor) :
        actor(actor),
        chanceOfRandomChoice(chanceOfRandomChoice){}

    EpisodeResult run() {
      while (!environment.done()) {
        Act act = actorLearn();
        lossRecords.push_back(act.loss);
        actionHistory.push_back(act.action);
      }
      return EpisodeResult{environment.score(), actionHistory, lossRecords};
    }

private:
    std::vector<int> actionHistory;
    actorClass& actor;
    double chanceOfRandomChoice;
    environmentClass environment;
    std::vector<double> lossRecords;

    Act actorLearn() {
      stateClass previousState = environment.state();
      int action = actor.proposeAction(previousState, chanceOfRandomChoice);
      environment.applyAction(action);
      actor.push(previousState, action, environment.state(), environment.reward(), environment.done());
      return Act{action, actor.learn()};
    }
};
