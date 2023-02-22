#pragma once

#include <cmath>
#include <algorithm>
#include <vector>


double epsilonByProportion(const double p)
{
  static constexpr double epsilonStart = 1.0;
  static constexpr double epsilonFinal = 0.01;
  static constexpr double epsilonDecay = 0.2;
//  static constexpr double epsilonDecay = 0.0001;
//  static constexpr int episodes = 20000;
//  static constexpr double epsilonMin = 0.01;

//  double chanceOfRandomChoice = 0.2 - epsilonDecay * p *episodes;
//  if (chanceOfRandomChoice < epsilonMin)
//    chanceOfRandomChoice = epsilonMin;
//  return chanceOfRandomChoice;

  return epsilonFinal + (epsilonStart - epsilonFinal)
                        * exp(-p / epsilonDecay);
}

double average(std::vector<double> const& v){
  if(v.empty()){
  return 0;
  }

  auto const count = static_cast<double>(v.size());
  return std::reduce(v.begin(), v.end()) / count;
}
