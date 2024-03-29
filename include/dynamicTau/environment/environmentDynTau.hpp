#pragma once

#include <cmath>

// Random coupling generation
#include <cstdlib>
#include <ctime>

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

	int stepCtr{};

	double startFreq;

public:

	BayesFactors bf;

	inline static const size_t numberOfPossibleActions = 2;

	explicit EnvironmentDynTau(EnvironmentDynTauConfiguration configuration = EnvironmentDynTauConfiguration{}) :
		configuration(configuration),
		timestep(configuration.maxTime / configuration.maxSteps), 
		bf(),
		stepCtr{},
		startFreq{}
		{
			reset();
		}


	void reset() override{
		bf.init("parameters.txt");

		// Randomly select a target coupling between 0.02 and 0.07
		srand(time(NULL));
		bf.targetCoupling = ((double)rand()/(double)RAND_MAX)/20+0.02;

		startFreq = bf.centerFreq;
	}

	void reset(std::string name){
		bf.init(name);
		startFreq = bf.centerFreq;
	}

	Feature state() const override{
		return bf.getState();
	}

	bool done() const override{
		return bf.startIndex > bf.rewardEndIndex;
	}

  	double score() const override{
		return bf.getScore();
	}

	double reward() const override{
		return done() ? bf.getReward() : 0;
	}

	void applyAction(int action) override{
		bf.step(action);
		stepCtr++;
	}
};

