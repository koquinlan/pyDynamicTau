#pragma once

#include <torch/torch.h>
#include <map>
#include <random>
#include <sstream>
#include <iomanip>

#include "Network.hpp"
#include "ReplayBuffer.hpp"
#include "Util.hpp"
#include "learner/reinforcement_learner/Actor.hpp"

#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/environment/environmentDynTau.hpp"

class ActorDynTau : Actor<Feature> {

  static constexpr double learningRate = 0.001;
  static constexpr double gamma = 0.98;
  static constexpr int numHidden = 8;
  static constexpr int minSampleSize = 20;
  static constexpr double tau = 0.8;

  Network dqn;
  Network target;

  ReplayBuffer<Feature> replayBuffer;
  torch::optim::Adam *tdOptimizer;
  std::minstd_rand *generator;

  torch::Tensor tdLoss(const Sample<Feature>& sample);

  void Construct(int seed) {
      generator = new std::minstd_rand(seed);
      torch::set_default_dtype(torch::scalarTypeToTypeMeta(torch::kDouble));

      dqn = Network(Feature().size(), numHidden, EnvironmentDynTau::numberOfPossibleActions);
      dqn->initializeParameters();
      tdOptimizer = new torch::optim::Adam(dqn->parameters(),
                                           torch::optim::AdamOptions(learningRate));

      target = Network(Feature().size(), numHidden, EnvironmentDynTau::numberOfPossibleActions);
      for (auto& p : target->named_parameters()) {
          torch::NoGradGuard no_grad;
          p.value().copy_(dqn->named_parameters()[p.key()]);
      }
  };

public:
    ActorDynTau() {
        std::random_device rd;
        Construct(rd.operator()());
    }
  explicit ActorDynTau(int seed)
  {
      Construct(seed);
  }

  ~ActorDynTau()
  {
    free(generator);
    free(tdOptimizer);
  }

  void push(const Feature& state, const int action, const Feature& next_state,
    const double reward, const bool done)
  {
    replayBuffer.push(state, action, next_state, reward, done);
  }

  int proposeAction(const Feature& state, const double epsilon);

  double learn();

  void save(std::string fileName) {
    torch::save(dqn, fileName + ".dqn");
    torch::save(target, fileName + ".tgt");
  }

  void load(std::string fileName) {
    torch::load(dqn, fileName + ".dqn");
    torch::load(target, fileName + ".tgt");
  }
};

int optimumPolicy(const std::vector<double>& qValue)
{
  std::vector<double>::const_iterator choice =
    std::max_element(qValue.begin(), qValue.end());
  return std::distance(qValue.begin(), choice);
}

int randomPolicy(size_t numAction, std::minstd_rand* generator)
{
  // // Unbiased random policy
  // return (*generator)() % numAction;

  // Bias the generation towards 0; P(0) = bias+1/(bias)
  int bias = 0;

  // Generate a random number between 0 and numAction+bias (exclusive)
  return (((*generator)() % (numAction+bias)) == (numAction+bias-1)) ? 1 : 0;

}

int ActorDynTau::proposeAction(const Feature& state, const double epsilon)
{
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  double chance = distribution(*generator);
  if (chance >= epsilon) {
    std::vector<double> qValue = toVector(
      dqn->forward(toTensor(state).to(torch::kDouble)));
    return optimumPolicy(qValue);
  } else
    return randomPolicy(EnvironmentDynTau::numberOfPossibleActions, generator);
}

torch::Tensor ActorDynTau::tdLoss(const Sample<Feature>& sample)
{
  torch::Tensor states, actions, next_states, rewards, dones;
  torch::Tensor qVals, next_actions, next_qVals, Y;

  std::tie(states, actions, next_states, rewards, dones) = toTensor(sample);
  qVals = dqn->forward(states).gather(1, actions);
  next_actions = dqn->forward(next_states).argmax(1, true);
  next_qVals = target->forward(next_states).gather(1, next_actions);
  Y = rewards + gamma * dones.logical_not() * next_qVals;

  return torch::mse_loss(qVals, Y.detach());
}

double ActorDynTau::learn()
{
  if (replayBuffer.size() < minSampleSize)
    return 0;
  Sample<Feature> sample = replayBuffer.sample(generator);
  torch::Tensor loss = tdLoss(sample);
  tdOptimizer->zero_grad();
  loss.backward(); 
  tdOptimizer->step();

  for (auto &p : target->named_parameters()) {
    torch::NoGradGuard no_grad;
    p.value().copy_(tau * p.value() + (1 - tau)
      * dqn->named_parameters()[p.key()]);
  }

  return loss.item<double>();
}
