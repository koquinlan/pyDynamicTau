#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

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

static void printTrialStepInfo(std::vector<TrialResult>& results) {
   int currMax=0;
   int sum=0;
   for (const TrialResult& trialResult : results) {
        if (trialResult.actionHistory.size() > currMax){
            currMax = trialResult.actionHistory.size();
        }
        sum += trialResult.actionHistory.size();
    }

    std::cout << "Average step count: " << sum/results.size() << std::endl;
    std::cout << "Max step count: " << currMax << std::endl;
}
