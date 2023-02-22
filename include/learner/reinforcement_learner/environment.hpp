#pragma once

template<class stateType>
class Environment {
public:
    virtual void reset() = 0;
    virtual stateType state() const = 0;
    virtual double score() const = 0;
    virtual bool done() const = 0;
    virtual double reward() const = 0;
    virtual void applyAction(int action) = 0;

    static const size_t numberOfPossibleActions;
};
