#include "./include/proc.hpp"

/**
 * @brief 
 * 
 * @return std::vector<double> 
 */
void Processor::generateLineshape(int lineshapeType, int freqRes){

    std::vector<double> lineshapeFreqRange;
    std::vector<double> lineshape;


    // 0 = sharpPeak, a spectrally sharp dirac function only on the axion frequency
    if (lineshapeType == 0){
        lineshapeWeights.push_back(1);
    }


    // 1 = axionLineshape, the axion lineshape spread over a range of 3 kHz with a resolution of 100 Hz
    else if (lineshapeType == 1){
        // set the frequency range/resolution
        for(int i=0; i<=10e3; i+=freqRes){
            lineshapeFreqRange.push_back(i);
        }

        // generate lineshape, y-axis meaningless
        double threeOverNuaBetaSquared = (3*3e8*3e8/(7e9*270e3*270e3)); // This line shape is for 7 GHz, changes very slightly at different frequencies
        for(int i=0; i<lineshapeFreqRange.size(); i++){
            lineshape.push_back(M_2_SQRTPI*sqrt(lineshapeFreqRange[i])*pow(threeOverNuaBetaSquared,1.5)*exp(-threeOverNuaBetaSquared*lineshapeFreqRange[i]));
        }

        // convert lineshape into weights that multiply a total axion power
        double cumSum = std::accumulate(std::begin(lineshape), std::end(lineshape), 0.0);
        for(int i=0; i<lineshape.size(); i++){
            lineshapeWeights.push_back(lineshape[i]/cumSum);
        }
    }

    else{
        std::cout << "lineshapeWeight must be either 1 for sharp peak or 2 for normal lineshape" << std::endl;
        exit(1);
    }
}



/**
 * @brief Simulates a set of raw data traces with no axions (assumes gaussian noise)
 * 
 * @param freqRange Frequency range to simulate the traces on (units of MHz)
 * @param numSpectra How many raw spectra to generate
 * @param centerFreq Absolute frequency that the raw traces should center on (units of MHz) (used later for shifting)
 * @param injectAxion Boolean int controls whether the trace contains an axion (1=axion, 0=normal)
 * @param axionFreq Absolute frequency that the axion should be generated at (units of MHz) (does nothing if injectAxion=0)
 * @param axionScaleFactor Axion size compared to cavity noise (analog to g_gamma)
 * @param gammaGain Characteristic width of the lorentzian gain profile
 * @param maxGain Peak gain
 * @param sigmaProc Standard deviation of the processed spectrum. Corresponds to how "fuzzy" the raw traces are, sigma_proc = 1/sqrt(bin_width*tau)
 * @return std::vector<Spectrum> 
 */
std::vector<Spectrum> Processor::simulateRaw(std::vector<double> freqRange, int numSpectra=10, double centerFreq = 7e3, int injectAxion=0, double axionFreq = 7.003e3, double axionScaleFactor = 1, double gammaGain=1, double maxGain=40.0, double sigmaProc=0.15){
    // Create a vector to hold SNRs and a vector to hold the raw spectra
    std::vector<double> fakeGain(freqRange.size());
    std::vector<double> fakeSNR(freqRange.size());
    std::vector<std::vector<double>> rawSpectra(numSpectra, std::vector<double> (freqRange.size()));
    
    // Initialize Random Generator to draw standard normals
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    // std::default_random_engine generator(std::random_device{}());
    std::normal_distribution<double> distribution(0,1.0);

    // Loop through frequency generating a lorentzian gain profile and corresponding SNR
    // (could probably move this to a seperate func and pass it in as an arg to save time)
    for(int i=0; i<freqRange.size(); i++){
        fakeGain[i] = (maxGain*gammaGain*M_PI_2)*(M_1_PI)*(0.5*gammaGain)/(pow(freqRange[i],2)+pow(gammaGain/2,2));
        fakeSNR[i] = fakeGain[i]/(1+fakeGain[i]);
    }

    // Loop through creating Spectrum objects for each raw spectrum and storing relevant information
    std::vector<Spectrum> rawSpectraObjects(rawSpectra.size());

    // If the axion will not be placed on this raw spectra, just generate normally
    if(injectAxion == 0 || axionFreq < (centerFreq+freqRange.front()) || axionFreq > (centerFreq+freqRange.back())){
        for(int i=0; i<rawSpectra.size(); i++){
            for(int j=0; j<rawSpectra[0].size(); j++){
                // Each point has mean 1+G, and standard deviation stdDev*(1+G)
                rawSpectra[i][j] = (distribution(generator))*(sigmaProc*(1+fakeGain[j]))+(1+fakeGain[j]);
            }

            rawSpectraObjects[i].setPowers(rawSpectra[i]);
            rawSpectraObjects[i].setGain(fakeGain);
            rawSpectraObjects[i].setSNR(fakeSNR);
            rawSpectraObjects[i].setFreqRange(freqRange);
            rawSpectraObjects[i].trueCenterFreq = centerFreq;
            rawSpectraObjects[i].gGamma = 0;
            rawSpectraObjects[i].sigmaProc = sigmaProc;
        }
    }
    // Otherwise, find where the axion goes and place it
    else{
        double spacing = freqRange[1]-freqRange[0];
        for(int i=0; i<rawSpectra.size(); i++){
            for(int j=0; j<rawSpectra[0].size(); j++){
                // If the axion frequency is closer to this bin than the distance between bins, place the axion starting at this freq
                if(std::abs((freqRange[j]+centerFreq-axionFreq))<spacing){
                    int foo = j;
                    for(int k=foo; (k<rawSpectra[0].size()) && (k<foo+lineshapeWeights.size()); k++){
                        rawSpectra[i][k] = (distribution(generator))*(sigmaProc*(1+fakeGain[j]))+(1+fakeGain[j]*(1+axionScaleFactor*lineshapeWeights[k-foo]));
                        j++;
                    }
                    j--;    
                }
                else{
                    // Each point has mean 1+Gain, and standard deviation stdDev*(1+Gain)
                    rawSpectra[i][j] = (distribution(generator))*(sigmaProc*(1+fakeGain[j]))+(1+fakeGain[j]);
                }
            }

            rawSpectraObjects[i].setPowers(rawSpectra[i]);
            rawSpectraObjects[i].setGain(fakeGain);
            rawSpectraObjects[i].setSNR(fakeSNR);
            rawSpectraObjects[i].setFreqRange(freqRange);
            rawSpectraObjects[i].trueCenterFreq = centerFreq;
            rawSpectraObjects[i].gGamma = axionScaleFactor;
            rawSpectraObjects[i].sigmaProc = sigmaProc;
        }
    }

    return rawSpectraObjects;
    
}



/**
 * @brief Converts a vector of raw spectra into a vector of processed spectra. This involves dividing by the baseline and subtracting 1. 
 * @note Final version needs to use a savitsky-golay or similar to get baseline. We currently just use the known SNR
 * 
 * @param rawSpectra Raw spectra to convert
 * @return std::vector<Spectrum> Vector of processed spectra
 */
std::vector<Spectrum> Processor::rawToProcessed(std::vector<Spectrum> rawSpectra){
    std::vector<Spectrum> processedSpectra = rawSpectra;
    SPECTRALOOP(processedSpectra){
        processedSpectra[i].powers[j] /= (1+processedSpectra[i].gain[j]);
        processedSpectra[i].powers[j] -= 1;
    }
    
    return processedSpectra;
}



/**
 * @brief Converts a vector of processed spectra into a vector of rescaled spectra. This involves rescaling by the std dev in each bin
 * 
 * @param processedSpectra Processed spectra to convert
 * @return std::vector<Spectrum> Vector of rescaled spectra
 */
std::vector<Spectrum> Processor::processedToRescaled(std::vector<Spectrum> processedSpectra){
    std::vector<Spectrum> rescaledSpectra = processedSpectra;
    for (int i=0; i < rescaledSpectra.size(); i++){
        double stddev = stdev(rescaledSpectra[i].powers);

        for (int j=0; j < rescaledSpectra[i].powers.size(); j++){
            rescaledSpectra[i].powers[j] /= (stddev*rescaledSpectra[i].SNR[j]);
        }
    }
    
    return rescaledSpectra;
}



combinedSpectrum Processor::combinedToGrand(combinedSpectrum combined, int rebinWidth=1){
    std::vector<double> grandPowers;
    std::vector<double> grandSigmas;
    std::vector<double> grandFreq;


    if(rebinWidth == 1){
        int grandWidth = lineshapeWeights.size();
        double newPower, newSigma;

        for(int i=0; i<combined.powers.size()-grandWidth; i++){
            newSigma=0;
            newPower=0;
            for(int j=0; j<grandWidth; j++){
                newSigma += (lineshapeWeights[j]*lineshapeWeights[j])/(combined.sigmaCombined[i+j]*combined.sigmaCombined[i+j]);
                newPower += lineshapeWeights[j]*combined.powers[i+j]/(combined.sigmaCombined[i+j]*combined.sigmaCombined[i+j]*grandWidth);
            }
            newSigma = grandWidth/pow(newSigma,0.5);
            newPower *= newSigma*newSigma;

            grandSigmas.push_back(newSigma);
            grandPowers.push_back(newPower);
        }

        grandFreq = combined.freqRange;
        grandFreq.erase(grandFreq.end() - grandWidth, grandFreq.end());
    }

    combinedSpectrum grandSpec;
    grandSpec.powers = grandPowers;
    grandSpec.freqRange = grandFreq;
    grandSpec.sigmaCombined = grandSigmas;

    return grandSpec;
}



std::vector<double> Processor::grandToBayes(combinedSpectrum grand, double axionMean){
    std::vector<double> bayesFactors(grand.powers.size());

    // Loop through grand spectrum pwoer excesses calculating u_i as in Palken 20 eq. 9
    SPECTRUMLOOP(grand){
        bayesFactors[i] = exp(axionMean*grand.powers[i] - axionMean*axionMean/2);
    }

    return bayesFactors;
}



/**
 * @brief Helper function to calculate the standard deviation of the values in a vector
 * 
 * @param v Vector to find std dev of
 * @return double Std dev of the vector input
 */
double Processor::stdev(std::vector<double> v){
    double sum = std::accumulate(std::begin(v), std::end(v), 0.0);
    double m =  sum / v.size();

    double accum = 0.0;
    std::for_each (std::begin(v), std::end(v), [&](const double d) {
        accum += (d - m) * (d - m);
    });

    double stdev = sqrt(accum / (v.size()-1));

    return stdev;
}