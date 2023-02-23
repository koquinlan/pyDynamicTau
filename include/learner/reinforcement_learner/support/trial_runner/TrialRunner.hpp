#pragma once

#include "../episode_runner/EpisodeResult.hpp"
#include "../episode_runner/EpisodeRunner.hpp"
#include "../Cacher.hpp"
#include "TrialResult.hpp"
#include "../../learnerUtil.hpp"

static const int UNINITIALIZED_FIDELITY = -1;
static TrialResult UNINITIALIZED_TRIAL_RESULT = TrialResult{UNINITIALIZED_FIDELITY, std::vector<int>{}, 0.0};

template<class actorClass, class stateClass, class environmentClass>
class TrialRunner {
public:
    double epsilon;

    TrialRunner(int trialNumber, int totalNumberOfTrials, int totalNumberOfEpisodes, actorClass& actor) :
            actor(actor),
            epsilon(epsilonByProportion(trialNumber / (double) totalNumberOfTrials)),
            totalNumberOfEpisodes(totalNumberOfEpisodes) {}

    TrialResult run() {
      return cacher.useCache<TrialResult>(__FUNCTION__, [this]() {
          std::vector<TrialResult> trialResults = trialResultsForEachEpisode();
          return getTrialResultWithMaximumFidelity(trialResults);
      });
    }

private:
    actorClass& actor;
    int totalNumberOfEpisodes;
    Cacher cacher;

    std::vector<TrialResult> trialResultsForEachEpisode() {
      std::vector<TrialResult> trialResults;
      for (const EpisodeResult& episodeResult : episodeResults()) {
          const TrialResult trialResult = TrialResult{episodeResult.fidelityAchieved,
                                                      episodeResult.actionHistory,
                                                      averageLossAcrossEpisodes()};
          trialResults.push_back(trialResult);
      }
      return trialResults;
    }

    double averageLossAcrossEpisodes() {
      return cacher.useCache<double>(__FUNCTION__, [this]() {
        std::vector<double> averageLossInEachEpisode;
        for (const EpisodeResult& episodeResult : episodeResults()) {
          averageLossInEachEpisode.push_back(average(episodeResult.lossPerAction));
        }
        return average(averageLossInEachEpisode);
      });
    }

    std::vector<EpisodeResult> episodeResults() {
      return cacher.useCache<std::vector<EpisodeResult>>(__FUNCTION__, [this]() {
        std::vector<EpisodeResult> episode_results;
        for (int _ = 0; _ < totalNumberOfEpisodes; _++) {
          episode_results.push_back(runEpisode());
        }
        return episode_results;
      });
    }

    [[nodiscard]] EpisodeResult runEpisode() const {
      return EpisodeRunner<actorClass, stateClass, environmentClass>{epsilon, actor}.run();
    }
};
