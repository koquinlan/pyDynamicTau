#include <iostream>
#include <fstream>

#include "actor.hpp"
#include "environment.hpp"
#include "util.hpp"

constexpr int numTrial = 100;
constexpr int numEpisode = 500;

int main()
{
  Environment environment;
  Actor<Feature, environment.numAction> actor;
//  actor.load("param");
  std::vector<int> saveAction, currentAction;
  double maxFid = 0;
  for (int trial = 0; trial < numTrial; trial++) {
    double epsilon = epsilonByProportion(trial / (double)numTrial);
    double avgLoss = 0;
    double maxFidTrial = 0;
    int lossCtr = 0;
    for (int episode = 0; episode < numEpisode; episode++) {
      currentAction.clear();
      environment.reset();
      Feature state = environment.state();
      bool done = environment.done();
      while (!done) {
        int action = actor.proposeAction(state, epsilon);
        currentAction.push_back(action);
        environment.applyAction(action);
        double reward = environment.reward();
        Feature next_state = environment.state();
        done = environment.done();
        actor.push(state, action, next_state, reward, done);
        avgLoss += actor.learn();
        lossCtr++;
        state = next_state;
      }
      maxFidTrial = std::max(maxFidTrial, environment.fidelity());
      if (maxFid < environment.fidelity()) {
        maxFid = environment.fidelity();
        saveAction = currentAction;
      }
    }
    std::cout << trial << "\tEpsilon: " << epsilon << "\tAvgloss: " 
      << avgLoss / lossCtr << "\tMaxFidTrial: " << maxFidTrial
      << "\tMaxFid: " << maxFid << std::endl;
  }
  actor.save("param");

  environment.reset();
  for (auto &p : saveAction)
    environment.applyAction(p);
  std::cout << "Fidelity : " << environment.fidelity() << std::endl;

  std::ofstream out("action.txt");
  for (auto &p : saveAction) {
    out << p << std::endl;
    std::cout << p << std::endl;
  }
  out.close();

  exit(0);
}
