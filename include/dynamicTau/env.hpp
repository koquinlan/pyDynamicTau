#pragma once

#include "bayes.hpp"

class Environment {

	BayesFactors bf;
	int stepCtr;

	double startFreq;

	public:

	static const size_t numAction = 2;

	void reset();

	Feature getState() const;
	bool done() const;
	double reward() const;
	int stepCount() const;
	void applyAction(int action);

};
