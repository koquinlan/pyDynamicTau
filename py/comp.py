import torch

# Load the .dqn file
module = torch.jit.load("param.tgt")

# Print the names of all the methods in the module
for name in dir(module):
    member = getattr(module, name)
    if callable(member):
        print(f"{name} is callable")
