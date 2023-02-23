#pragma once

#include <cmath>

#include "bayes.hpp"
#include "learner/reinforcement_learner/environment.hpp"

struct EnvironmentDynTauConfiguration {
    int maxSteps = 20;
    double threshold = 0.95;
    double maxTime = 5;
};

class EnvironmentDynTau : Environment<Feature> {

	const EnvironmentDynTauConfiguration configuration;
	const double timestep;

	BayesFactors bf;
	int stepCtr{};

	double startFreq;

public:

	inline static const size_t numberOfPossibleActions = 2;

	explicit EnvironmentDynTau(EnvironmentDynTauConfiguration configuration = EnvironmentDynTauConfiguration{}) :
		configuration(configuration),
		timestep(configuration.maxTime / configuration.maxSteps) {}


	void reset() override{
		bf.init("parameters.txt");
		startFreq = bf.centerFreq;
	}

	Feature state() const override{
		return bf.getState();
	}

	bool done() const override{
		return bf.centerFreq > (startFreq + bf.scanningWindowSize);
	}

  	double score() const override{
		return bf.getReward();
	}

	double reward() const override{
		return done() ? bf.getReward() : 0;
	}

	void applyAction(int action) override{
		bf.step(action);
		stepCtr++;
	}
};

