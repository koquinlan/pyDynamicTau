#pragma once

#include <vector>

template <class stateClass>
class classicDynTau {

    double targetCoupling;
    double threshold;
    stateClass points;

public:
    classicDynTau(stateClass SNR) {
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
            points[i] = SNR[i+1] + SNR[i]/norm;
        }
    }

    void setThreshold(){
        threshold = 0;
        for(int i=0; i < (int)points.size()/3; i++){
            threshold += points[i];
        }
    }
};