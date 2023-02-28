#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

#include <vector>
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>

#include "dynamicTau/environment/environmentDynTau.hpp"
#include "dynamicTau/environment/bayes.hpp"
#include "dynamicTau/Util.hpp"

// Define the input size and output size of the RL agent
const int INPUT_SIZE = 14;
const int OUTPUT_SIZE = 2;

std::ostream& operator<<(std::ostream& out, const torch::jit::script::Module& module) {
    out << "Module:\n";
    out << "  Parameters:\n";
    for (const auto& parameter : module.named_parameters()) {
        out << "    " << parameter.name() << " : " << parameter.value().sizes() << "\n";
    }
    out << "  Buffers:\n";
    for (const auto& buffer : module.named_buffers()) {
        out << "    " << buffer.name() << " : " << buffer.value().sizes() << "\n";
    }
    out << "  Children:\n";
    for (const auto& child : module.named_children()) {
        out << "    " << child.name() << " : " << child.value() << "\n";
    }
    out << "  Methods:\n";
    for (const auto& method : module.get_methods()) {
        out << "    " << method.name() << "()\n";
    }
    return out;
}

int main() {
    try {
        // Load the script module
        torch::jit::script::Module module = torch::jit::load("param.dqn");

        // Set the module to evaluation mode
        module.eval();

        // Print the module's architecture
        std::cout << module << std::endl;
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

//     // Check what methods are included in the loaded agent
//     auto methods = module.get_methods();
//     for (const auto& method : methods) {
//         std::cout << method.name() << std::endl;
//     }



//     // Create a vector of input values for the RL agent
//     EnvironmentDynTau env;
//     env.reset();
//     env.applyAction(0);

//     std::cout << "Starting loop" << std::endl;
//     while(!env.done()){
//         // Use the RL agent to choose an action based on the input tensor
//         try {
//             auto input_tensor = toTensor(env.state());
            
//             std::cout << "Input tensor shape: " << input_tensor.sizes() << std::endl;
//             std::cout << "Input tensor data type: " << input_tensor.dtype() << std::endl;

//             auto output_tensor = module.forward({input_tensor}).toTensor();
//             auto output = output_tensor.argmax(/*dim=*/1).item<int>();

//             std::cout << "Chosen action: " << output << std::endl;

//             env.applyAction(output);

//         } catch (const c10::Error& e) {
//             std::cerr << "An error occurred: " << e.what() << std::endl;
//         }
//     }

//     Feature state = env.state();
//     std::vector<double> vecState(state.begin(), state.end());

//     plt::plot(vecState);

//     // plt::plot(bf.fullFreqRange, bf.exclusionLine);

//     plt::show();

//     return 0;
// }
