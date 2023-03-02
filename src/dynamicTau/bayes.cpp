#include "dynamicTau/environment/bayes.hpp"

/**
 * @brief Call initParams to initalize parameters and set the data to it's starting value
 * 
 * @param parametersFile Text file to pass into initParams, lines of the form 'name=value'
 */
void BayesFactors::init(const std::string& parametersFile){
    // Clear any existing data
    fullFreqRange.clear();
    freqRange.clear();

    aggregateUpdate.clear();
    exclusionLine.clear();

    coeffSumA.clear();
    coeffSumB.clear();

    initParams(parametersFile);

    // Sharp axion peak
    gGammaFactors = gGammaSim;

    // Create axion lineshape in processor (0-sharp peak, 1-axion lineshape)
    proc.generateLineshape(0, freqRes);

    // Populates the freqRange and initializes the BayesFactors and exclusion line
    for(int i=-freqWindowSize/2; i<=freqWindowSize/2; i+=freqRes){
        freqRange.push_back(i/1e6);
        fullFreqRange.push_back(i/1e6);

        // Set mechanics for the Bayes factors calc
        aggregateUpdate.push_back(1);

        // Set mechanics for exclusion line calc
        exclusionLine.push_back(100);
        coeffSumA.push_back(0);
        coeffSumB.push_back(0);
    }

    // Determine the window over which the reward should be calculated
    double endFreq = centerFreq+scanningWindowSize;
    rewardStartIndex = (freqRange.size()-1)/2;
    rewardEndIndex = rewardStartIndex+scanningWindowSize*(1e6)/freqRes;
}



/**
 * @brief Read input from a parameters.txt file to initialize class variables
 * 
 * @param parametersFile Text file with lines of the form 'name=value'
 */
void BayesFactors::initParams(const std::string& parametersFile){
    std::map<std::string, std::string> parameters;

    // Parse a parameter file for key,value pairs (key=value)
    std::ifstream file(parametersFile);
    if(file.is_open()){
        std::string line;
        while(std::getline(file, line)){
            // Skip lines starting with # (comments)
            if(line[0] == '#'){
                continue;
            }

            // Split line on = sig and read pair
            size_t pos = line.find('=');
            if(pos == std::string::npos){
                continue;
            }
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos+1);

            // Store in parameters map
            parameters[key] = value;
        }
        file.close();
    }
    else{
        std::cout << "Warning: file " << parametersFile << " not opened." << std::endl;
    }

    // Set the starting values with defaults
    maxGain = parameters.count("maxGain") ? std::stod(parameters["maxGain"]) : 60.0;
    gammaGain = parameters.count("gammaGain") ? std::stod(parameters["gammaGain"]) : 1;

    gGammaSim = parameters.count("gGammaSim") ? std::stod(parameters["gGammaSim"]) : 0.1;
    axionFreq = parameters.count("axionFreq") ? std::stod(parameters["axionFreq"]) : 7e3;
    axionBool = parameters.count("axionBool") ? std::stoi(parameters["axionBool"]) : 1;

    sigmaProc = parameters.count("sigmaProc") ? std::stod(parameters["sigmaProc"]) : 0.1;
    nSpectra = parameters.count("nSpectra") ? std::stoi(parameters["nSpectra"]) : 10;

    freqWindowSize = parameters.count("freqWindowSize") ? std::stoi(parameters["freqWindowSize"]) : 10000000; //Hz
    freqRes = parameters.count("freqRes") ? std::stoi(parameters["freqRes"]) : 100; //Hz
    centerFreq = parameters.count("centerFreq") ? std::stod(parameters["centerFreq"]) : 7e3-1; //MHz

    tuningStepFreq = parameters.count("tuningStepFreq") ? std::stod(parameters["tuningStepFreq"]) : 2; //MHz
    scanningWindowSize = parameters.count("scanningWindowSize") ? std::stod(parameters["scanningWindowSize"]) : 5; //MHz

    targetCoupling = parameters.count("targetCoupling") ? std::stod(parameters["targetCoupling"]) : 0.01;

    // Deprecated reward idea params
    belowThresholdValue = parameters.count("belowThresholdValue") ? std::stod(parameters["belowThresholdValue"]) : -1000;
    aboveThresholdValue = parameters.count("aboveThresholdValue") ? std::stod(parameters["aboveThresholdValue"]) : 60;
    stepPunish = parameters.count("stepPunish") ? std::stod(parameters["stepPunish"]) : -5;

    // Tanh reward params
    peakReward = parameters.count("peakReward") ? std::stod(parameters["peakReward"]) : 60;
    peakSharpness = parameters.count("peakSharpness") ? std::stod(parameters["peakSharpness"]) : 1;

    startIndex = 0;
}



/**
 * @brief Advance the scanning weither by taking for data in place or stepping forward
 * 
 * @param stepForward Boolean integer - 0: stay in place, 1: step forward
 */
void BayesFactors::step(int stepForward){
    // If stepForward is true, move the center frequency of the run over by the tuningStepFreq size
    if(stepForward){
        double targetCenter = centerFreq + tuningStepFreq;
        while(centerFreq < targetCenter){
            centerFreq += freqRes/1e6;
            startIndex += 1;

            fullFreqRange.push_back(fullFreqRange.back()+freqRes/1e6);

            // Expand BayesFactor Vector
            aggregateUpdate.push_back(1);

            // Expand exclusion line vector
            exclusionLine.push_back(0);
            coeffSumA.push_back(0);
            coeffSumB.push_back(0);
        }
    }

    // Calculate a set of new Bayes factors based on the current simulation settings
    combinedSpectrum combined = genCombined();

    updateBayes(combined);
    updateExclusionLine(combined);
    updateState();
}



/**
 * @brief Return the current value of the reward
 * 
 * @return double reward
 */
double BayesFactors::getReward() const{
    double reward=0;

    if(exclusionLine.size() >= rewardEndIndex){
        std::vector<double> fullWindow(exclusionLine.begin()+rewardStartIndex, exclusionLine.begin()+rewardEndIndex);

        Feature fullState = windowMax(fullWindow, 2*state.size());

        for (int i=0; i<fullState.size(); i++){
            reward += rewardFunction(fullState[i])/(peakReward*fullState.size());
        }
    }

    return reward;
}



void BayesFactors::updateState(){
    std::vector<double> activeWindow(exclusionLine.begin()+startIndex, exclusionLine.end());

    if (activeWindow.size() < state.size()) {
        std::cout << ("Exclusion line is smaller than input size.") << std::endl;
    }
    else if (activeWindow.size() > state.size()) {
        state = windowMax(activeWindow, state.size());
    }
    else{
        std::copy(activeWindow.end() - state.size(), activeWindow.end(), state.begin());
    }
}



Feature BayesFactors::getState() const{
    Feature normState;

    // Normalize exclusions to target coupling
    for (int i=0; i<state.size(); i++){
        normState[i] = state[i]/targetCoupling;
    }

    return normState;
}



/**
 * @brief Calculates the reward for whatever the current state is (usually the max in a binned version of the active scanning window)
 * 
 * @return double Reward for the current state
 */
double BayesFactors::getScore() const{
    double score = 0;

    for (int i=0; i<state.size(); i++){
        score += rewardFunction(state[i])/(peakReward*state.size());
    }

    return score;
}



/**
 * @brief Iterates over the input vector and takes the max in each bin to produce an output Feature of size ouputSize
 * 
 * @param input Input vector to window max
 * @param outputSize Size for window maxed output
 * @return Feature Array of the max in each window of the input
 */
Feature BayesFactors::windowMax(const std::vector<double>& input, int outputSize) const{
    Feature output;

    // Calculate the size of each "bin"
    int binSize = input.size() / outputSize;

    // Loop over each bin
    for (int i = 0; i < outputSize-1; i++)
    {
        // Calculate the start and end indices of the current bin
        int windowStartIndex = i * binSize;
        int windowEndIndex = windowStartIndex + binSize;

        // Calculate the sum of the elements in the current bin
        double max = 0.0;
        for (int j = windowStartIndex; j < windowEndIndex; j++)
        {
            if (max < input[j]){
                max = input[j];
            }
        }

        // Add the max to the output vector
        output[i] = max;
    }

    // Calculate the start and end indices of the last bin
    int windowStartIndex = (outputSize - 1) * binSize;
    int windowEndIndex = input.size();

    // Calculate the sum of the elements in the last bin
    double max = 0.0;
    for (int j = windowStartIndex; j < windowEndIndex; j++)
    {
        if (max < input[j]){
            max = input[j];
        }
    }

    // Add the max to the output vector
    output[outputSize - 1] = max;

    return output;
}



/**
 * @brief Iterates over the input vector and takes the average in each bin to produce an output Feature of size ouputSize
 * 
 * @param input Input vector to window average
 * @param outputSize Size for window averaged output
 * @return Feature Array of the average in each window of the input
 */
Feature BayesFactors::windowAverage(const std::vector<double>& input, int outputSize) const{
    Feature output;

    // Calculate the size of each "bin"
    int binSize = input.size() / outputSize;
    int remainder = input.size() % outputSize;

    // Loop over each bin
    for (int i = 0; i < outputSize-1; i++)
    {
        // Calculate the start and end indices of the current bin
        int windowStartIndex = i * binSize;
        int windowEndIndex = windowStartIndex + binSize;

        // Calculate the sum of the elements in the current bin
        double sum = 0.0;
        for (int j = windowStartIndex; j < windowEndIndex; j++)
        {
            sum += input[j];
        }

        // Calculate the average of the elements in the current bin
        double average = sum / binSize;

        // Add the average to the output vector
        output[i] = average;
    }

    // Calculate the start and end indices of the last bin
    int windowStartIndex = (outputSize - 1) * binSize;
    int windowEndIndex = input.size();

    // Calculate the sum of the elements in the last bin
    double sum = 0.0;
    for (int j = windowStartIndex; j < windowEndIndex; j++)
    {
        sum += input[j];
    }

    // Calculate the average of the elements in the last bin
    double average = sum / (binSize + remainder);

    // Add the average to the output vector
    output[outputSize - 1] = average;

    return output;
}



/**
 * @brief Move the 90% exclusion line to it's new coupling strengths based on updated information
 * 
 * @param combined combinedSpectrum object containing the data to update the exclusion cut with
 */
void BayesFactors::updateExclusionLine(combinedSpectrum combined){
    double coeffA, coeffB;
    double scanFactor, newExcludedStrength;
    double fourLnPtOne = 9.210340372; // Numerical factor 4*ln(0.1) that goes into quadratice formula, 0.1 set by desried exclusion level (90%)

    for(int i=0; i<combined.powers.size(); i++){
        scanFactor = (1/sigmaProc)*sqrt(combined.weightSum[i]);

        coeffSumA[startIndex+i] += scanFactor*scanFactor/2;
        coeffSumB[startIndex+i] += scanFactor*combined.powers[i]/combined.sigmaCombined[i];

        coeffA = coeffSumA[startIndex+i];
        coeffB = coeffSumB[startIndex+i];

        newExcludedStrength = (coeffB+std::sqrt(coeffB*coeffB+fourLnPtOne*coeffA))/(2*coeffA);

        exclusionLine[startIndex+i] = newExcludedStrength;
    }
}



/**
 * @brief 
 * 
 * @param exclusionStrength 
 * @return double 
 */
double BayesFactors::rewardFunction(double excludedCoupling) const{

    return 1/(std::abs(std::tanh(peakSharpness*(excludedCoupling-targetCoupling)))+(1/peakReward));
}



/**
 * @brief Update the aggregate bayes factors based on incoming data
 * 
 * @param combined combinedSpectrum object containing the data to update the aggregate bayes factors with
 */
void BayesFactors::updateBayes(combinedSpectrum combined){
    double expectedMean;
    double factor;

    for(int i=0; i<combined.powers.size(); i++){
        expectedMean = gGammaFactors*(1/sigmaProc)*sqrt(combined.weightSum[i]);
        factor = exp(expectedMean*combined.powers[i]/combined.sigmaCombined[i]-expectedMean*expectedMean/2);

        aggregateUpdate[startIndex+i] *= factor;
    }
}



/**
 * @brief Generate a new combinedSpectrum object based on simulation parameters
 * 
 * @return combinedSpectrum simulated combined spectrum based on current parameters
 */
combinedSpectrum BayesFactors::genCombined(){
    // Initialize a combined spectrum object
    combinedSpectrum combined;
    combined.sigmaProc = sigmaProc;
    combined.gGamma = gGammaSim;

    // Simulate a number of raw traces
    std::vector<Spectrum> raw = proc.simulateRaw(freqRange, nSpectra, centerFreq, axionBool, axionFreq, gGammaSim, gammaGain, maxGain, sigmaProc);

    // Combine the raw traces into one combined spectrum
    std::vector<Spectrum> processed = proc.rawToProcessed(raw);
    std::vector<Spectrum> rescaled = proc.processedToRescaled(processed);
    
    for(int j=0; j<rescaled.size(); j++){
        combined.addRescaledSpectrum(rescaled[j]);
    }

    return combined;
}