import ctypes
import numpy as np

class Environment:
    def __init__(self):
        self.lib = ctypes.cdll.LoadLibrary('../build/release/dynTauEnv.dll')
        if not self.lib:
            print("Failed to load DLL")
        else:
            print("DLL loaded successfully")

        # cast the return value of env_new to Environment*
        self.env = self.lib.env_new()
        self.env = ctypes.cast(self.env, ctypes.c_void_p)


        self.lib.env_reset.argtypes = [ctypes.c_void_p]
        self.lib.env_getState.argtypes = [ctypes.c_void_p]
        self.lib.env_getState.restype = ctypes.POINTER(ctypes.c_double * 5)
        self.lib.env_applyAction.argtypes = [ctypes.c_void_p, ctypes.c_int]
        self.lib.env_getReward.argtypes = [ctypes.c_void_p]
        self.lib.env_checkDone.argtypes = [ctypes.c_void_p]
        self.lib.env_stepCount.argtypes = [ctypes.c_void_p]

    def reset(self):
        self.lib.env_reset(ctypes.byref(self.env))

    def getState(self):
        state_ptr = self.lib.env_getState(ctypes.byref(self.env))
        state_arr = np.ctypeslib.as_array(state_ptr.contents)
        return state_arr

    def applyAction(self, action):
        self.lib.env_applyAction(ctypes.byref(self.env), action)

    def getReward(self):
        return self.lib.env_getReward(ctypes.byref(self.env))

    def done(self):
        return self.lib.env_checkDone(ctypes.byref(self.env))

    def stepCount(self):
        return self.lib.env_stepCount(ctypes.byref(self.env))

    def __del__(self):
        self.lib.env_delete(ctypes.byref(self.env))



# Create an instance of the Environment class
env = Environment()

# Call member functions
state = env.getState()

# env.reset()
# state = env.getState()
# done = env.done()
# reward = env.getReward()
# step_count = env.stepCount()
# env.applyAction(0)
