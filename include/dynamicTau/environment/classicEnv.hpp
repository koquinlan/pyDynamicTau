#pragma once

#include <cmath>

// Random coupling generation
#include <cstdlib>
#include <ctime>

#include "bayes.hpp"

class ClassicEnvDynTau {

	double startFreq;

public:

	BayesFactors bf;
	inline static const size_t numberOfPossibleActions = 2;

	explicit ClassicEnvDynTau() :
		bf(),
		startFreq{}
		{
			reset();
		}


	void reset() {
		bf.init("parameters.txt");

		startFreq = bf.centerFreq;
	}

	void reset(std::string name){
		bf.init(name);
		startFreq = bf.centerFreq;
	}

	Feature state() const {
		return bf.getState();
	}

    Feature stateAxis() const {
        return bf.getStateAxis();
    }

	bool done() const {
		return bf.startIndex > bf.rewardEndIndex;
	}

  	double score() const {
		return bf.getScore();
	}

	double reward() const {
		return done() ? bf.getReward() : 0;
	}

	void applyAction(int action) {
		bf.step(action);
	}
};
