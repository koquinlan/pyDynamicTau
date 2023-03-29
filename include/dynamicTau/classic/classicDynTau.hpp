#pragma once

#include <vector>

template <class stateClass>
class classicDynTau {

public:
    double targetCoupling;
    double threshold;
    stateClass points;
    std::vector<double> targets;

    double targetRescaling = 0.8;
    double thresholdRescaling = 1.365;
    double thresholdTarget = 2.7;

    double lowerWindow = 0.4;
    double upperWindow = 0.747;

    int minScans = 0;
    int currScans = 0;

    int absMinScans = 2;

    classicDynTau(){
        
    }

    classicDynTau(stateClass SNR, double target) {
        targetCoupling = target;
        
        setPoints(SNR);
        setTargets(SNR);
        setThreshold();
    }

    int proposeAction(stateClass& state, int scanCount){
        if (scanCount < absMinScans){
            return 0;
        }
        // std::cout << checkScore(state) << " vs. " << threshold << std::endl;

        int action = (checkScore(state) < threshold);
        // std::cout << "Selected action: " << action << std::endl;

        if (action){
            currScans++; 
        }
        else{
            currScans = 0;
        }

        return action && (currScans > minScans);
    }

    double checkScore(stateClass& state){
        double score = 0;

        for (int i=0; i < state.size(); i++){
            if (state[i] > targets[i]) score += points[i]*std::pow(state[i]/targets[i], 2);
        }
        
        return score;
    }

    void setPoints(stateClass& SNR){
        /** OPTION ONE - rescaled inverse SNR **/

        // double norm = SNR[SNR.size()-1];
        // points[points.size()-1] = 1;

        // for(int i=SNR.size()-2; i>=0; i--){
        //     points[i] = points[i+1] + SNR[i]/norm;
        // }


        /** OPTION TWO - manually prevent back tail **/

        // for(int i=0; i<points.size(); i++){
        //     points[i] = 1;
        // }


        /** OPTION THREE - only reward for center region **/

        // for(int i=0; i<points.size(); i++){
        //     points[i] = (i >= 2*points.size()/5) && (i <= 3*points.size()/5);
        // }


        /** OPTION FOUR - reward for center 70% with inverse scaled square SNR **/
        double SNRsum=0;
        double cumSum=0;

        for (int i=0; i<SNR.size(); i++){
            SNRsum += SNR[i]*SNR[i];
        }

        int j = SNR.size()-1;
        while(cumSum/SNRsum < lowerWindow){
            cumSum += SNR[j]*SNR[j];
            points[j] = 0;
            j--;
        }

        double norm = SNR[j]*SNR[j];
        while(cumSum/SNRsum < upperWindow){
            cumSum += SNR[j]*SNR[j];

            points[j] = points[j+1] + SNR[j]*SNR[j]/norm;
            j--;
        }

        while(j >= 0){
            points[j]=0;
            j--;
        }
    }

    void setTargets(stateClass& SNR){
        double SNRsum=0;
        for (int i=0; i<SNR.size(); i++){
            SNRsum += SNR[i]*SNR[i];
            targets.push_back(0);
        }

        double cumSum=0;
        for (int i=targets.size()-1; i>=0; i--){
            cumSum += SNR[i]*SNR[i];
            targets[i] = (std::sqrt(SNRsum/cumSum)*targetCoupling - targetCoupling);

            if ((targetRescaling*std::sqrt(SNRsum/cumSum))>1){
                targets[i] /= (targetRescaling*std::pow(SNRsum/cumSum,2));
            }

            targets[i] += targetCoupling;
        }
    }

    void setThreshold(){
        /** OPTION ONE - rescaled inverse SNR **/

        // threshold = 0;
        // for(int i=(points.size()-1); i > (int)((points.size()-1)/2); i--){
        //     threshold += points[i];
        // }

        // threshold += points[0];
        // threshold += points[1];

        /** OPTION TWO - manually prevent back tail **/
        
        // threshold = 1;

        /** OPTION THREE - Portion of total points **/
        int i=0;

        while(points[i] == 0){
            i++;
        }

        int j=i;

        while(points[j+1] != 0){
            j++;
        }        

        threshold = points[i+(int)((j-i)/thresholdTarget)]/thresholdRescaling;

        // i = (int)points.size()/2;

        // threshold = points[i]*std::pow(((targets[i] - targetCoupling)*targetRescaling+targetCoupling)/targets[i], 1)/thresholdRescaling;
        // // threshold += points[i+1]/2;
    }
};