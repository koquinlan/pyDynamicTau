#pragma once

#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <iostream>
#include <fstream>
#include <iomanip> 
#include <string>

#include "dynamicTau/environment/bayes.hpp"
#include "matplotlibcpp.h"

//#include <wx/wx.h>


int main(){
    std::cout << "Inside main" << std::endl;

    // Initialize an instance of the BayesFactors class
    BayesFactors bf;
    bf.init("../parameters.txt");

    // Loop until the user chooses to exit
    while(true){
        std::cout << "Enter 1 to take more data, 2 to step forward or 0 to exit: ";
        int choice;
        std::cin >> choice;

        if(choice == 0){
            break;
        }
        else if(choice == 1){
            bf.step(false);
        }
        else if(choice == 2){
            bf.step(true);      
        }
        else{
            std::cout << "Invalid choice. Please enter 1/2 to scan or 0 to exit." << std::endl;
            continue;
        }

        Feature state = bf.getState();
        std::vector<double> vecState(state.begin(), state.end());

        plt::plot(vecState);

        // plt::subplot(2, 1, 1);
        // plt::plot(bf.fullFreqRange, bf.exclusionLine);

        // plt::subplot(2, 1, 2);
        // plt::plot(bf.fullFreqRange, bf.exclusionLine);

        plt::show();
    }

    return 0;
}