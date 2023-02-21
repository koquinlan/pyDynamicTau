#pragma once

#include "bayes.hpp"

class Environment {

	BayesFactors bf;
	int stepCtr;

	double startFreq;

	public:

	static const size_t numAction = 2;

	void reset(){
		bf.init("parameters.txt");
		startFreq = bf.centerFreq;
	}

	Feature getState() const
	{
		return bf.getState();
	}

	/**
	 * @brief Check whether the desired window has been scanned over
	 * 
	 * @return true - center frequency past desired scanning window (scan complete)
	 * @return false - scan incomplete
	 */
	bool done() const{
		return bf.centerFreq > (startFreq + bf.scanningWindowSize);
	}

	double reward() const
	{
		return done() ? bf.getReward() : 0;
	}

	int stepCount() const {
		return stepCtr;
	}

	void applyAction(int action) {
		bf.step(action);
		stepCtr++;
	}

};
