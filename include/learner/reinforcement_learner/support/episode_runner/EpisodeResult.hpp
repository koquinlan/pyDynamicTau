#pragma once

#include <numeric>

struct EpisodeResult {
  double fidelityAchieved;
  std::vector<int> actionHistory;
  std::vector<double> lossPerAction;

  double totalLoss() {
      return std::accumulate(lossPerAction.begin(), lossPerAction.end(), 0.0);
  }
};
