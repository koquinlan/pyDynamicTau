#pragma once

#include <cmath>

#include "LatticeBeamsplitter.hpp"
#include "learner/reinforcement_learner/Environment.hpp"

struct EnvironmentBeamsplitterConfiguration {
    int maxSteps = 20;
    double threshold = 0.95;
    double maxTime = 5;
};

class EnvironmentBeamsplitter : Environment<Feature> {

  const EnvironmentBeamsplitterConfiguration configuration;
  const double timestep;
  constexpr static const double phi [] = {-M_PI, -M_PI/2, -M_PI/4, 0, M_PI/2};

  LatticeBeamsplitter lattice;
  int stepCtr{};

public:

  inline static const size_t numberOfPossibleActions = std::end(phi) - std::begin(phi);

  explicit EnvironmentBeamsplitter(EnvironmentBeamsplitterConfiguration configuration = EnvironmentBeamsplitterConfiguration{}) :
      configuration(configuration),
      timestep(configuration.maxTime / configuration.maxSteps) {}

  void reset() override
  {
    stepCtr = 0;
    lattice.groundState();
  }

  Feature state() const override
  {
    return lattice.feature();
  }

  double score() const override
  {
    return lattice.fidelity();
  }

  bool done() const override
  {
    return stepCtr >= configuration.maxSteps || score() > configuration.threshold;
  }

  double reward() const override
  {
    double fid = lattice.fidelity();
    return done() ? fid / (1 - fid) : 0;
  }

  void applyAction(int action) override
  {
    lattice.step(phi[action], timestep);
    stepCtr++;
  }

};
