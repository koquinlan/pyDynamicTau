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
        // if (env.bf.startIndex > env.bf.rewardStartIndex/2){
        if (1){
            choice = algo.proposeAction(env.state(), scanCount);
        }
        else{
            choice = (scanCount >= 10);
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

    plt::ylim((double)0, rescale/4);
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
    // Create directory to save info
    std::string foldername = "scanningRun";
    help.createFolder(foldername);


    // Decompose full range into the reward window and corresponding x-axis
    std::vector<double> fullWindow(env.bf.exclusionLine.begin()+env.bf.rewardStartIndex, env.bf.exclusionLine.begin()+env.bf.rewardEndIndex);
    std::vector<double> fullAxis(env.bf.fullFreqRange.begin()+env.bf.rewardStartIndex, env.bf.fullFreqRange.begin()+env.bf.rewardEndIndex);

    help.saveState(fullAxis, fullWindow, "finalState", foldername);


    // Save stepInfo
    help.saveState(stepInfo, "stepInfo", foldername);
}

void ScanRunner::saveCheckpoint(int checkpointNum){
    // Create folder to store data
    HelperUtil help;
    std::string foldername = "checkpoint" + checkpointNum;

    help.createFolder(foldername);


    // Get and save SNR vector
    Feature stateSNR = env.bf.windowAverage(env.bf.SNR, env.state().size());
    std::vector<double> fullSNR = env.bf.SNR;

    help.saveState(stateSNR, "stateSNR", foldername);
    help.saveState(fullSNR, "fullSNR", foldername);


    // Get and save current state observation vector
    Feature state = env.state();
    Feature stateAxis = env.stateAxis();

    help.saveState(state, "state", foldername);
    help.saveState(stateAxis, "stateAxis", foldername);


    // Get and save full range
    std::vector<double> activeWindow(env.bf.exclusionLine.begin()+env.bf.startIndex, env.bf.exclusionLine.end());
    std::vector<double> activeAxis(env.bf.fullFreqRange.begin()+env.bf.startIndex, env.bf.fullFreqRange.end());

    help.saveState(activeAxis, "fullAxis", foldername);
    help.saveState(activeWindow, "fullState", foldername);


    // Get and save information about the algorithm
    help.saveState(algo.points, "points", foldername);
    help.saveState(algo.targets, "targets", foldername);
}