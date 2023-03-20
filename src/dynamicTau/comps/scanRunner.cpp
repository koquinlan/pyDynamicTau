#include "dynamicTau/classic/scanRunner.hpp"

void ScanRunner::runScan(int plotting){
    while (applyChoice(makeChoice())){
        if (plotting) showState(plotting-1);
    }
}

int ScanRunner::makeChoice(){
    int choice;

    #ifdef MANUAL
        choice = queryChoice();
    #else
        if (env.bf.startIndex > env.bf.rewardStartIndex/2){
        // if (0){
            choice = algo.proposeAction(env.state());
        }
        else{
            choice = (scanCount >= 12);
        }
    #endif

    return choice;
}

int ScanRunner::queryChoice(){
    int choice;

    std::cout << "Enter 1 to take more data, 2 to step forward or 0 to exit: ";
    std::cin >> choice;
    choice --;

    return choice;
}

int ScanRunner::applyChoice(int choice){
    if(choice == -1 || env.done()){
        return 0;
    }

    env.applyAction(choice);
    if (choice){
        if (env.bf.startIndex > env.bf.rewardStartIndex/2){
            std::cout << "Stepping forward." << std::endl;

            stepInfo.push_back(scanCount);
        }
        else{
            std::cout << "Smoothing start condition" << std::endl;
        }

        scanCount = 0;
    }
    else{
        scanCount++;
    }

    return 1;
}

void ScanRunner::showState(int continuousPlotting){
    // std::cout << "Current score: " << algo.checkScore(env.state()) << " vs. threshold " << algo.threshold << std::endl;
    Feature state = env.state();
    Feature stateAxis = env.stateAxis();


    // Get the current state (discrete maxes) and the full exclusion line in the active window
    std::vector<double> vecState(state.begin(), state.end());
    std::vector<double> vecStateAxis(stateAxis.begin(), stateAxis.end());

    std::vector<double> activeWindow(env.bf.exclusionLine.begin()+env.bf.startIndex, env.bf.exclusionLine.end());
    std::vector<double> activeAxis(env.bf.fullFreqRange.begin()+env.bf.startIndex, env.bf.fullFreqRange.end());

    std::vector<double> SNR = env.bf.SNR;


    // Rescale SNR to fit within the plotting window
    // double rescale = std::max_element(vecState.begin(), vecState.end())[0];
    double rescale = 0.4;

    for (int i = 0; i < SNR.size(); i++) SNR[i] *= rescale;


    // Adjust targets to only show threshold with nonzero point value
    std::vector<double> realTargets;
    for (int i=0; i<algo.targets.size(); i++){
        realTargets.push_back((algo.points[i]>0)*algo.targets[i]);
    }


    // Plot everything on the same axis
    plt::clf();

    plt::plot(activeAxis, activeWindow);
    plt::plot(vecStateAxis, realTargets);
    plt::plot(activeAxis, std::vector<double> (activeAxis.size(), env.bf.targetCoupling));
    plt::plot(activeAxis, SNR);
    plt::scatter(vecStateAxis, vecState, 20, {{"color", "red"}});

    plt::ylim((double)0, rescale);
    plt::title("Simulated Scanning Run");


    // Either pause until graph is closed or continuously display
    if (continuousPlotting) plt::pause(0.0001);
    else plt::show();
}

void ScanRunner::showFinal(){
    // Print info about requested scan numbers
    std::cout << "Total scans requested: " << std::accumulate(stepInfo.begin(), stepInfo.end(), 0) << std::endl;
    std::cout << "Average scans requested: " << (double)std::accumulate(stepInfo.begin(), stepInfo.end(), 0)/(double)stepInfo.size() << std::endl;
    std::cout << "Total steps taken: " << stepInfo.size() << std::endl;

    // Decompose full range into the reward window and corresponding x-axis
    std::vector<double> fullWindow(env.bf.exclusionLine.begin()+env.bf.rewardStartIndex, env.bf.exclusionLine.begin()+env.bf.rewardEndIndex);
    std::vector<double> fullAxis(env.bf.fullFreqRange.begin()+env.bf.rewardStartIndex, env.bf.fullFreqRange.begin()+env.bf.rewardEndIndex);

    // Plot full axis and target coupling
    plt::clf();

    plt::plot(fullAxis, fullWindow);
    plt::plot(fullAxis, std::vector<double> (fullAxis.size(), env.bf.targetCoupling));
    plt::show();
}

void ScanRunner::saveFinal(){
    // Decompose full range into the reward window and corresponding x-axis
    std::vector<double> fullWindow(env.bf.exclusionLine.begin()+env.bf.rewardStartIndex, env.bf.exclusionLine.begin()+env.bf.rewardEndIndex);
    std::vector<double> fullAxis(env.bf.fullFreqRange.begin()+env.bf.rewardStartIndex, env.bf.fullFreqRange.begin()+env.bf.rewardEndIndex);


    // Save final scanning result
    std::string stateFilename = "finalState.csv";
    std::ofstream stateFile(stateFilename);

    if (!stateFile.is_open()) {
        std::cerr << "Error: unable to open file " << stateFilename << "\n";
        return;
    }
    for (size_t i = 0; i < fullAxis.size(); i++) {
        stateFile << fullAxis[i] << "," << fullWindow[i] << "\n";
    }
    stateFile.close();


    // Save scanning step information
    std::string stepFilename = "stepInfo.csv";
    std::ofstream stepFile(stepFilename);

    if (!stepFile.is_open()) {
        std::cerr << "Error: unable to open file " << stepFilename << "\n";
        return;
    }
    for (size_t i = 0; i < stepInfo.size(); i++) {
        stepFile << stepInfo[i] << "\n";
    }
    stepFile.close();


    // Create formatted name for the folder
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "scanningRun_" << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d_%H-%M-%S");
    std::string folderName = ss.str();
    std::string folderPath = "../" + folderName;

    // Create folder to store info
    if (!std::filesystem::create_directory(folderPath)) {
        std::cerr << "Error: unable to create folder " << folderPath << "\n";
        return;
    }


    // Move the stored vectors to the created folder
    if (std::filesystem::exists(folderPath)) {
        std::filesystem::rename(stateFilename, folderName + "/" + stateFilename);
        std::filesystem::rename(stepFilename, folderName + "/" + stepFilename);
    } else {
        std::cerr << "Error: unable to move files to folder\n";
    }
}