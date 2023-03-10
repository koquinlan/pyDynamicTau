#pragma once

#include <vector>

template <class stateClass>
class classicDynTau {

public:
    double targetCoupling;
    double threshold;
    stateClass points;

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
            if (state[i] > targetCoupling) score += points[i];
        }
        
        return score;
    }

    void setPoints(stateClass& SNR){
        double norm = SNR[SNR.size()-1];
        points[points.size()-1] = 1;

        for(int i=SNR.size()-2; i>=0; i--){
            points[i] = points[i+1] + SNR[i]/norm;
        }
    }

    void setThreshold(){
        threshold = 0;
        for(int i=(points.size()-1); i > (int)((points.size()-1)/2.4); i--){
            threshold += points[i];
        }
    }
};