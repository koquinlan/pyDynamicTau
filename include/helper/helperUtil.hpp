#pragma once

#include <vector>
#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <chrono>
#include <iomanip>

class HelperUtil {

public:
    void createFolder(std::string name, int appendTime=1){
        std::string folderPath = "../" + name;

        if (appendTime){folderPath += "_" + getTime();}

        // Create folder to store info
        if (!std::filesystem::create_directory(folderPath)) {
            std::cerr << "Error: unable to create folder " << folderPath << "\n";
        }
    }

    std::string getTime(){
        // Use chrono to return date and time as a string
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d_%H-%M-%S");

        return ss.str();
    }

    template <class State>
    void saveState(State state, std::string filename = "default", std::string foldername = ""){
        std::string fullname = "../" + foldername + "/" + filename + ".csv";
        std::ofstream file(fullname);

        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " << fullname << "\n";
            return;
        }

        if(state.size() == 0){
            std::cerr << "Size of state to save is 0" << "\n";
            return;
        }

        for (size_t i = 0; i < state.size()-1; i++) {
            file << state[i] << ",";
        }

        file << state[state.size()];

        file.close();
    }


    template <class State>
    void saveState(State state1, State state2, std::string filename = "default", std::string foldername = ""){
        std::string fullname = "../" + foldername + "/" + filename + ".csv";
        std::ofstream file(fullname);

        if (!file.is_open()) {
            std::cerr << "Error: unable to open file " << fullname << "\n";
            return;
        }

        if(state1.size() == 0 || state2.size() == 0){
            std::cerr << "Size of state to save is 0" << "\n";
            return;
        }

        if(state1.size() != state2.size()){
            std::cerr << "Size of the state is not the same, try svaing seperately" << "\n";
            return;
        }

        for (size_t i = 0; i < state1.size(); i++) {
            file << state1[i] << "," << state2[i] << "\n";
        }

        file.close();
    }
};