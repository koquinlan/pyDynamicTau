#pragma once

#include <iostream>
#include <fstream>
#include <chrono>

#include "support/trial_runner/TrialRunner.hpp"
#include "support/episode_runner/EpisodeRunner.hpp"

#define TIMING (1)

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
            
            #if TIMING
              auto now = std::chrono::system_clock::now();
              auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
              auto now_time_t = std::chrono::system_clock::to_time_t(now);
              std::cout << "Trial " << trialNumber << " " <<
              "started at " << std::put_time(std::localtime(&now_time_t), "%F %T") << 
              "." << now_ms.time_since_epoch().count() % 1000 << std::endl;

              std::chrono::high_resolution_clock::time_point trialStart = std::chrono::high_resolution_clock::now();
            #endif

            TrialRunner<actorClass, stateClass, environmentClass> trial{trialNumber,
                                                                        totalNumberOfTrials,
                                                                        totalNumberOfEpisodes,
                                                                        actor};
            TrialResult trialResult = trial.run();
            if (trialResult.fidelityAchieved > maxFidelityTrial.fidelityAchieved) {
              maxFidelityTrial = trialResult;
            }
            logTrial(trialNumber, maxFidelityTrial.fidelityAchieved, trialResult, trial.epsilon);

            #if TIMING
              std::chrono::high_resolution_clock::time_point trialStop = std::chrono::high_resolution_clock::now();
              std::chrono::duration<double, std::nano> trialTime = std::chrono::duration_cast<std::chrono::nanoseconds>(trialStop-trialStart);

              std::cout << "Trial " << trialNumber << " runtime: " << trialTime.count()/(1e9) << " seconds" << std::endl;

            #endif
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
