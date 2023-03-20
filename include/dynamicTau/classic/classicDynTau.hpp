#pragma once

#include <vector>

template <class stateClass>
class classicDynTau {

public:
    double targetCoupling;
    double threshold;
    stateClass points;
    std::vector<double> targets;

    classicDynTau(){
        
    }

    classicDynTau(stateClass SNR, double target) {
        targetCoupling = target;
        
        setPoints(SNR);
        setTargets(SNR);
        setThreshold();
    }

    int proposeAction(stateClass& state){
        // std::cout << checkScore(state) << " vs. " << threshold << std::endl;

        int action = (checkScore(state) < threshold);
        // std::cout << "Selected action: " << action << std::endl;

        return action;
    }

    double checkScore(stateClass& state){
        double score = 0;

        for (int i=0; i < state.size(); i++){
            if (state[i] > targets[i]) score += points[i]*(state[i]/targets[i])*(state[i]/targets[i]);
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
        while(cumSum/SNRsum < 0.3){
            cumSum += SNR[j]*SNR[j];
            points[j] = 0;
            j--;
        }


        double norm = SNR[j]*SNR[j];
        while(cumSum/SNRsum < 0.7){
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
            targets[i] = (std::sqrt(SNRsum/cumSum)*targetCoupling - targetCoupling)/2 + targetCoupling; 
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

        threshold = points[i]/1.3;
        // threshold += points[i+1]/2;
    }
};