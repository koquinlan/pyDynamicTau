#pragma once

#include <iostream>
#include <fstream>

#include "support/trial_runner/TrialRunner.hpp"
#include "support/episode_runner/EpisodeRunner.hpp"

template<class actorClass, class stateClass, class environmentClass>
class ReinforcementLearner {
public:
    ReinforcementLearner(int totalNumberOfTrials, int totalNumberOfEpisodes) :
        totalNumberOfTrials(totalNumberOfTrials),
        totalNumberOfEpisodes(totalNumberOfEpisodes) {}
    ReinforcementLearner(int totalNumberOfTrials, int totalNumberOfEpisodes, int seed) :
        totalNumberOfTrials(totalNumberOfTrials),
        totalNumberOfEpisodes(totalNumberOfEpisodes),
        actor(actorClass{seed}) {}

    double run() {
      return getFinalScoreOnFreshEnvironment();
    }

    void printResults() {
      printFinalScore();
      printActionsTaken();
    }

private:
    int totalNumberOfTrials;
    int totalNumberOfEpisodes;
    actorClass actor;
    Cacher cacher;

    void printFinalScore() {
      std::cout << "Fidelity : " << getFinalScoreOnFreshEnvironment() << std::endl;
    }

    double getFinalScoreOnFreshEnvironment() {
      return cacher.useCache<double>(__FUNCTION__, [this]() {
          environmentClass environment;
          for (auto &p : bestActionHistory())
            environment.applyAction(p);
          return environment.score();
      });
    }

    void printActionsTaken() {
      std::ofstream out("action.txt");
      for (auto &p : bestActionHistory()) {
        out << p << std::endl;
        std::cout << p << std::endl;
      }
      out.close();
    }

    std::vector<int> bestActionHistory() {
      return getTrialWithMaximumFidelity().actionHistory;
    }

    TrialResult getTrialWithMaximumFidelity() {
      return cacher.useCache<TrialResult>(__FUNCTION__, [this]() {
          TrialResult maxFidelityTrial = UNINITIALIZED_TRIAL_RESULT;
          for (int trialNumber = 0; trialNumber < totalNumberOfTrials; trialNumber++) {
            TrialRunner<actorClass, stateClass, environmentClass> trial{trialNumber,
                                                                        totalNumberOfTrials,
                                                                        totalNumberOfEpisodes,
                                                                        actor};
            TrialResult trialResult = trial.run();
            if (trialResult.fidelityAchieved > maxFidelityTrial.fidelityAchieved) {
              maxFidelityTrial = trialResult;
            }
            logTrial(trialNumber, maxFidelityTrial.fidelityAchieved, trialResult, trial.epsilon);
          }
          actor.save("param");
          return maxFidelityTrial;
      });
    }

    void logTrial(int trialNumber, double currentMaxFidelity, const TrialResult& trialResult, double epsilon) {
      std::cout << trialNumber << "\tEpsilon: " << epsilon << "\tAvgloss: "
                << trialResult.averageLossPerAction << "\tMaxFidTrial: " << trialResult.fidelityAchieved
                << "\tMaxFid: " << currentMaxFidelity << std::endl;
    }
};
