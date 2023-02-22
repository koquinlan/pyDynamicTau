#include "env.hpp"
#include <vector>


extern "C" {
    __declspec(dllexport) void* env_new() {
        return new Environment();
    }

    __declspec(dllexport) void env_reset(Environment* env){
        env->reset();
    }

    __declspec(dllexport) std::vector<double> env_getState(Environment* env) {
        return env->getState();
    }

    __declspec(dllexport) void env_applyAction(Environment* env, int action) {
        env->applyAction(action);
    }

    __declspec(dllexport) double env_getReward(Environment* env) {
        return env->reward();
    }

    __declspec(dllexport) bool env_checkDone(Environment* env){
        return env->done();
    }

    __declspec(dllexport) int env_stepCount(Environment* env){
        return env->stepCount();
    }

    __declspec(dllexport) void env_delete(Environment* env) {
        delete env;
    }
}


