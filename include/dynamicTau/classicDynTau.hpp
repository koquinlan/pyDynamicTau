#pragma once

#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/environment/environmentDynTau.hpp"

template <class stateClass>
class {

    double targetCoupling;
    double threshold;
    stateClass points;

public:
    int proposeAction(stateClass& state){
        if (checkScore(state) > threshold){
            return 0;
        }
        else{
            return 1;
        }
    }

    void setPoints(stateClass& SNR){
        double norm = SNR[SNR.size()-1];
        points[points.size()-1] = 1;

        for(int i=SNR.size()-2; i>=0; i--){
            points[i] = SNR[i+1] + SNR[i]/norm;
        }
    }

    double checkScore(stateClass& state){
        
    }
}