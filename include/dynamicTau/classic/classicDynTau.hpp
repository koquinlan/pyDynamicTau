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
        setThreshold();
    }

    int proposeAction(stateClass& state){
        return (checkScore(state) < threshold);
    }

    double checkScore(stateClass& state){
        double score = 0;

        for (int i=0; i < state.size(); i++){
            if (state[i] > targets[i]) score += points[i];
        }
        
        return score;
    }

    void setPoints(stateClass& SNR){
        /** OPTION ONE - rescaled inverse SNR **/

        // double norm = SNR[SNR.size()-1];
        // points[points.size()-1] = 1;
        // targets.push_back(targetCoupling);

        // for(int i=SNR.size()-2; i>=0; i--){
        //     points[i] = points[i+1] + SNR[i]/norm;
        //     targets.push_back(targetCoupling);
        // }

        /** OPTION TWO - manually prevent back tail **/

        // for(int i=0; i<points.size(); i++){
        //     points[i] = (i<=(points.size()-1)/2);
        //     targets.push_back(targetCoupling);
        // }


        /** OPTION THREE - On track to hit the target **/

        for(int i=0; i<points.size(); i++){
            points[i] = (i<=(points.size()-1)/2);
        }

        double SNRsum=0;
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
        
        threshold = 2;
    }
};