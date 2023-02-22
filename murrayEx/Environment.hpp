#pragma once

#include <cmath>

#include "lattice.hpp"

class Environment {

  const int maxSteps = 20;
  const double threshold = 0.95;
  const double maxTime = 5;
  const double dt = maxTime / maxSteps;
  const std::vector<double> phi {-M_PI, -M_PI/2, -M_PI/4, 0, M_PI/2};

  Lattice lattice;
  int stepCtr;

public:

  static const size_t numAction = 5;

  void reset()
  {
    stepCtr = 0;
    lattice.groundState();
  }

  Feature state() const
  {
    return lattice.feature();
  }

  double fidelity() const
  {
    return lattice.fidelity();
  }

  bool done() const
  {
    return stepCtr >= maxSteps || fidelity() > threshold;
  }

  double reward() const
  {
    double fid = lattice.fidelity();
    return done() ? fid / (1 - fid) : 0;
  }

  int stepCount() const
  {
    return stepCtr;
  }

  void applyAction(int action)
  {
    lattice.step(phi[action], dt);
    stepCtr++;
  }

};
