#include "./include/env.hpp"

void Environment::reset(){
    bf.init("parameters.txt");
    startFreq = bf.centerFreq;
}



Feature Environment::getState() const
{
    return bf.getState();
}



/**
 * @brief Check whether the desired window has been scanned over
 * 
 * @return true - center frequency past desired scanning window (scan complete)
 * @return false - scan incomplete
 */
bool Environment::done() const{
    return bf.centerFreq > (startFreq + bf.scanningWindowSize);
}



double Environment::reward() const
{
    return done() ? bf.getReward() : 0;
}



int Environment::stepCount() const {
    return stepCtr;
}



void Environment::applyAction(int action) {
    bf.step(action);
    stepCtr++;
}