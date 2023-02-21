#pragma once

#include <vector>
#include <cmath>
#include <random>

#include <chrono>
#include <numeric>
#include <algorithm>

#include <iostream>

#include "decs.hpp"

class Processor {
public:

    std::vector<double> lineshapeWeights;

    void generateLineshape(int lineshapeType, int freqRes);

    std::vector<Spectrum> simulateRaw(std::vector<double> freqRange, int numSpectra=10, double centerFreq = 7e3, 
                                      int injectAxion=0, double axionFreq = 7.003e3, double axionScaleFactor = 1, 
                                      double gammaGain=1, double maxGain=40.0, double sigmaProc=0.15); 

    std::vector<Spectrum> rawToProcessed(std::vector<Spectrum> rawSpectra);

    std::vector<Spectrum> processedToRescaled(std::vector<Spectrum> processedSpectra);

    combinedSpectrum combinedToGrand(combinedSpectrum combined, int rebinWidth=1);

    std::vector<double> grandToBayes(combinedSpectrum grand, double axionMean);

private:
    double stdev(std::vector<double> v);
};



class Spectrum {
public:
    std::vector<double> freqRange, powers, gain, SNR;

    double trueCenterFreq, gGamma, sigmaProc;

    Spectrum();

    Spectrum(std::vector<double> newRange);

    void setPowers(std::vector<double> newPowers);
    void setGain(std::vector<double> newGain);
    void setSNR(std::vector<double> newSNR);
    void setFreqRange(std::vector<double> newFreqRange);
};



class combinedSpectrum : public Spectrum{
public:
    std::vector<double> weightSum;
    std::vector<double> sigmaCombined;
    std::vector<int> numTraces;

    void addRescaledSpectrum(Spectrum rescaledSpectrum);
};