#pragma once

#include <vector>
#include <algorithm>

struct TrialResult {
    double fidelityAchieved;
    std::vector<int> actionHistory;
    double averageLossPerAction;
};

static bool compareFidelity(const TrialResult& s1, const TrialResult& s2) {
    return s1.fidelityAchieved < s2.fidelityAchieved;
}

static TrialResult& getTrialResultWithMaximumFidelity(std::vector<TrialResult>& results) {
    return *std::max_element(results.begin(), results.end(), compareFidelity);
}
