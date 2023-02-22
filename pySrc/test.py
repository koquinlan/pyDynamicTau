import ctypes

# Load the DLL
lib = ctypes.cdll.LoadLibrary('../build/release/dynTauEnv.dll')
if not lib:
    print("Failed to load DLL")
else:
    print("DLL loaded successfully")

# Print out all the attributes of the library object
print(dir(lib))