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
    bf.init("..\\..\\src\\parameters.txt");

    // // Initialize the plot
    // plt::plot(bf.fullFreqRange, bf.aggregateUpdate);
    // plt::show();

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

            // Update the plot with the new aggregateUpdate
            plt::plot(bf.fullFreqRange, bf.exclusionLine);
            plt::show();
        }
        else if(choice == 2){
            bf.step(true);

            // Update the plot with the new aggregateUpdate
            plt::plot(bf.fullFreqRange, bf.exclusionLine);
            plt::show();
        }
        else{
            std::cout << "Invalid choice. Please enter 1/2 to scan or 0 to exit." << std::endl;
        }
    }

    return 0;
}

// class MainFrame : public wxFrame {
// public:
//     MainFrame(const wxString& title);

//     void OnMoreDataButton(wxCommandEvent& event);
//     void OnStepForwardButton(wxCommandEvent& event);
//     void OnExitButton(wxCommandEvent& event);

// private:
//     BayesFactors bf;

//     wxButton *moreDataButton;
//     wxButton *stepForwardButton;
//     wxButton *exitButton;

//     wxDECLARE_EVENT_TABLE();
// };

// enum {
//     ID_MORE_DATA_BUTTON = 1,
//     ID_STEP_FORWARD_BUTTON = 2,
//     ID_EXIT_BUTTON = 3
// };

// wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
//     EVT_BUTTON(ID_MORE_DATA_BUTTON, MainFrame::OnMoreDataButton)
//     EVT_BUTTON(ID_STEP_FORWARD_BUTTON, MainFrame::OnStepForwardButton)
//     EVT_BUTTON(ID_EXIT_BUTTON, MainFrame::OnExitButton)
// wxEND_EVENT_TABLE()

// MainFrame::MainFrame(const wxString& title)
//     : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    
//     // Create three buttons for taking more data, stepping forward, and exiting
//     moreDataButton = new wxButton(this, ID_MORE_DATA_BUTTON, "Take More Data", wxPoint(50, 50));
//     moreDataButton->SetBackgroundColour(*wxGREEN);

//     stepForwardButton = new wxButton(this, ID_STEP_FORWARD_BUTTON, "Step Forward", wxPoint(300, 50));
//     stepForwardButton->SetBackgroundColour(*wxYELLOW);

//     exitButton = new wxButton(this, ID_EXIT_BUTTON, "Exit", wxPoint(550, 50));
//     exitButton->SetBackgroundColour(*wxRED);
// }

// void MainFrame::OnMoreDataButton(wxCommandEvent& event) {
//     if(bf.aggregateUpdate.empty()){
//         bf.init("parameters.txt");
//     }
//     bf.step(false);

//     // Update the plot with the new aggregateUpdate
//     plt::plot(bf.fullFreqRange, bf.exclusionLine);
//     plt::show();
// }

// void MainFrame::OnStepForwardButton(wxCommandEvent& event) {
//     if(bf.aggregateUpdate.empty()){
//         bf.init("parameters.txt");
//     }
//     bf.step(true);

//     // Update the plot with the new aggregateUpdate
//     plt::plot(bf.fullFreqRange, bf.exclusionLine);
//     plt::show();
// }

// void MainFrame::OnExitButton(wxCommandEvent& event) {
//     Close();
// }

// class App : public wxApp {
// public:
//     bool OnInit() override;
//     int OnRun() override;
// };

// bool App::OnInit() {
//     MainFrame *frame = new MainFrame("Bayes Factors");
//     frame->Show(true);
//     return true;
// }

// int App::OnRun() {
//     return wxApp::OnRun();
// }

// wxIMPLEMENT_APP(App);

// int main(int argc, char *argv[]) {
//     App app;
//     return wxEntry(argc, argv);
// }