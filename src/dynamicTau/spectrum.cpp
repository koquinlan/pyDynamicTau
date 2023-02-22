#include "proc.hpp"


Spectrum::Spectrum(){
}



Spectrum::Spectrum(std::vector<double> newRange){
    freqRange = newRange;
}



void Spectrum::setPowers(std::vector<double> newPowers){
    powers = newPowers;
}



void Spectrum::setGain(std::vector<double> newGain){
    gain = newGain;
}



void Spectrum::setSNR(std::vector<double> newSNR){
    SNR = newSNR;
}



void Spectrum::setFreqRange(std::vector<double> newFreqRange){
    freqRange = newFreqRange;
}



/**
 * @brief Updates the combined spectrum powers with new information (ie. more rescaled spectra coming in)
 * 
 * @param rescaledSpectrum Rescaled spectrum object to add to the combined spectrum
 */
void combinedSpectrum::addRescaledSpectrum(Spectrum rescaledSpectrum)
{
    std::vector<double> trueRescaledRange = rescaledSpectrum.freqRange;
    double shift = rescaledSpectrum.trueCenterFreq;

    // Shift the frequency range to be absolute rather than relative
    for(int i=0; i<trueRescaledRange.size(); i++){
        trueRescaledRange[i] += shift;
    }

    // Either set the combined spectrum frequency range and powers or expand it as necessary
    if(freqRange.empty()){
        freqRange = trueRescaledRange;
        powers = rescaledSpectrum.powers;
        for(int i=0; i<rescaledSpectrum.SNR.size(); i++){
            sigmaCombined.push_back(pow(pow(rescaledSpectrum.SNR[i],2),-0.5));
            weightSum.push_back(pow(rescaledSpectrum.SNR[i],2));
            numTraces.push_back(1);
        }
    }
    // Case where there may be a shift that needs to occur
    else{
        // Logic if the trace we're trying to add comes before the current combined spectrum for some reason
        // Note the performance here is horrible since we have to "push_front" a vector -- avoid this at all costs!!
        int shiftStart = 0;
        double currFront = freqRange.front();

        while(trueRescaledRange[shiftStart] < currFront){
            shiftStart += 1;

            // Maximum value of shiftStart is the final index of the rescaled range
            if(shiftStart==(trueRescaledRange.size())) break;
        }
        shiftStart = shiftStart-1; // Value of negative one corresponds to no dangling values, the two ranges overlap completely

        for(int i=shiftStart; i >= 0; i--){
            freqRange.insert(freqRange.begin(), 1, trueRescaledRange[i]);
            weightSum.insert(freqRange.begin(), 1, 0);
            sigmaCombined.insert(freqRange.begin(), 1, 0);
            powers.insert(freqRange.begin(), 1, 0);
        }


        // Logic if the trace we're trying to add comes after the current combined spectrum (this is the case when we step forward)
        int shiftBack = trueRescaledRange.size()-1;
        double currBack = freqRange.back();

        while(trueRescaledRange[shiftBack] > currBack){
            shiftBack -= 1;

            // Minimum value of shiftStart is 0 
            if(shiftBack==-1) break;     
        }
        shiftBack = shiftBack+1; // Value of trueRescaledRange.size() corresponds to no dangling values, the two ranges overlap completely

        for(int i=shiftBack; i <trueRescaledRange.size(); i++){
            freqRange.push_back(trueRescaledRange[i]);
            weightSum.push_back(0);
            sigmaCombined.push_back(0);
            powers.push_back(0);
        }



        // Now do the vertical recombination
        int fullRangeIndex = 0;
        while(freqRange[fullRangeIndex] != trueRescaledRange.front()){
            fullRangeIndex++;

            if(fullRangeIndex==freqRange.size()){
                std::cout << "An issue occured in adding this rescaled spectrum to the combined spectrum" << std::endl;
                exit(-1);
            }
        }

        double newSNRsq;
        double oldSum;
        double newSum;
        for(int i=0; i<trueRescaledRange.size(); i++){
            // Increase the number of contributing traces
            numTraces[i+fullRangeIndex] += 1;

            // Add SNR (R_ij) squared to the sum
            oldSum = weightSum[i+fullRangeIndex];
            newSNRsq = pow(rescaledSpectrum.SNR[i],2);
            newSum = oldSum+newSNRsq;

            // Update the sum normalization term and sigma in each bin
            weightSum[i+fullRangeIndex] = newSum;
            sigmaCombined[i+fullRangeIndex] = pow(1/newSum,0.5);

            // Update the powers based on the reweighted contrubting traces
            powers[i+fullRangeIndex] *= (oldSum/newSum);
            powers[i+fullRangeIndex] += (newSNRsq/newSum)*rescaledSpectrum.powers[i]; 
        }
    }
}