#pragma once

#include <string>

template <class stateClass>
class Actor {
public:
    virtual void push(const stateClass& state,
                      const int action,
                      const stateClass& next_state,
                      const double reward,
                      const bool done) = 0;

    virtual int proposeAction(const stateClass& state, const double epsilon) = 0;

    virtual double learn() = 0;

    virtual void save(std::string fileName) = 0;

    virtual void load(std::string fileName) = 0;
};
