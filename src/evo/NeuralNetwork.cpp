#include "evo/NeuralNetwork.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace TunnelStrike {

// Activation functions implementation
float NeuralNetwork::activate(float x, Activation func) {
    switch (func) {
        case Activation::SIGMOID:
            return 1.0f / (1.0f + std::exp(-x));
        case Activation::TANH:
            return std::tanh(x);
        case Activation::RELU:
            return std::max(0.0f, x);
        case Activation::LEAKY_RELU:
            return x > 0.0f ? x : x * 0.01f;
        case Activation::LINEAR:
            return x;
        default:
            return x;
    }
}

float NeuralNetwork::activateDerivative(float x, Activation func) {
    switch (func) {
        case Activation::SIGMOID: {
            float s = 1.0f / (1.0f + std::exp(-x));
            return s * (1.0f - s);
        }
        case Activation::TANH: {
            float t = std::tanh(x);
            return 1.0f - t * t;
        }
        case Activation::RELU:
            return x > 0.0f ? 1.0f : 0.0f;
        case Activation::LEAKY_RELU:
            return x > 0.0f ? 1.0f : 0.01f;
        case Activation::LINEAR:
            return 1.0f;
        default:
            return 1.0f;
    }
}

// Constructor
NeuralNetwork::NeuralNetwork(const Config& config)
    : config(config), rng(std::random_device{}()) {
    if (config.layers.size() < 2) {
        throw std::invalid_argument("Network must have at least input and output layers");
    }
    
    // Initialize layers
    layers.resize(config.layers.size());
    for (size_t i = 0; i < config.layers.size(); ++i) {
        unsigned layer_size = config.layers[i];
        layers[i].biases.resize(layer_size, 0.0f);
        layers[i].activations.resize(layer_size, 0.0f);
        layers[i].errors.resize(layer_size, 0.0f);
        layers[i].prev_biases_delta.resize(layer_size, 0.0f);
        
        if (i > 0) {
            unsigned prev_size = config.layers[i - 1];
            layers[i].weights.resize(layer_size, std::vector<float>(prev_size, 0.0f));
            layers[i].prev_weights_delta.resize(layer_size, std::vector<float>(prev_size, 0.0f));
        }
    }
    
    initializeWeights();
}

// Copy constructor
NeuralNetwork::NeuralNetwork(const NeuralNetwork& other)
    : config(other.config), layers(other.layers), experiences(other.experiences), rng(std::random_device{}()) {
}

// Assignment operator
NeuralNetwork& NeuralNetwork::operator=(const NeuralNetwork& other) {
    if (this != &other) {
        config = other.config;
        layers = other.layers;
        experiences = other.experiences;
    }
    return *this;
}

NeuralNetwork::~NeuralNetwork() = default;

// Initialize weights with Xavier/Glorot initialization
void NeuralNetwork::initializeWeights() {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    for (size_t i = 1; i < layers.size(); ++i) {
        unsigned layer_size = config.layers[i];
        unsigned prev_size = config.layers[i - 1];
        
        float range = std::sqrt(6.0f / (layer_size + prev_size));
        std::uniform_real_distribution<float> weight_dist(-range, range);
        
        for (unsigned j = 0; j < layer_size; ++j) {
            layers[i].biases[j] = weight_dist(rng);
            for (unsigned k = 0; k < prev_size; ++k) {
                layers[i].weights[j][k] = weight_dist(rng);
            }
        }
    }
}

// Forward pass
std::vector<float> NeuralNetwork::predict(const std::vector<float>& input) {
    if (input.size() != config.layers.front()) {
        throw std::invalid_argument("Input size doesn't match network input layer");
    }
    
    // Set input layer activations
    for (size_t i = 0; i < input.size(); ++i) {
        layers[0].activations[i] = input[i];
    }
    
    // Forward propagate through hidden layers
    for (size_t i = 1; i < layers.size() - 1; ++i) {
        unsigned layer_size = config.layers[i];
        unsigned prev_size = config.layers[i - 1];
        
        for (unsigned j = 0; j < layer_size; ++j) {
            float sum = layers[i].biases[j];
            for (unsigned k = 0; k < prev_size; ++k) {
                sum += layers[i].weights[j][k] * layers[i - 1].activations[k];
            }
            layers[i].activations[j] = activate(sum, config.hidden_activation);
        }
    }
    
    // Forward propagate through output layer
    size_t output_idx = layers.size() - 1;
    unsigned output_size = config.layers[output_idx];
    unsigned prev_size = config.layers[output_idx - 1];
    
    for (unsigned j = 0; j < output_size; ++j) {
        float sum = layers[output_idx].biases[j];
        for (unsigned k = 0; k < prev_size; ++k) {
            sum += layers[output_idx].weights[j][k] * layers[output_idx - 1].activations[k];
        }
        layers[output_idx].activations[j] = activate(sum, config.output_activation);
    }
    
    return layers[output_idx].activations;
}

float NeuralNetwork::predictSingle(const std::vector<float>& input) {
    auto output = predict(input);
    return output.empty() ? 0.0f : output[0];
}

// Backward pass for supervised learning
void NeuralNetwork::backwardPass(const std::vector<float>& target) {
    if (target.size() != config.layers.back()) {
        throw std::invalid_argument("Target size doesn't match network output layer");
    }
    
    size_t output_idx = layers.size() - 1;
    
    // Calculate output layer errors
    for (unsigned j = 0; j < config.layers[output_idx]; ++j) {
        float output = layers[output_idx].activations[j];
        float error = target[j] - output;
        layers[output_idx].errors[j] = error * activateDerivative(output, config.output_activation);
    }
    
    // Backpropagate through hidden layers
    for (int i = static_cast<int>(layers.size()) - 2; i >= 1; --i) {
        unsigned layer_size = config.layers[i];
        unsigned next_size = config.layers[i + 1];
        
        for (unsigned j = 0; j < layer_size; ++j) {
            float error = 0.0f;
            for (unsigned k = 0; k < next_size; ++k) {
                error += layers[i + 1].weights[k][j] * layers[i + 1].errors[k];
            }
            layers[i].errors[j] = error * activateDerivative(layers[i].activations[j], config.hidden_activation);
        }
    }
}

// Update weights using gradient descent with momentum
void NeuralNetwork::updateWeights() {
    for (size_t i = 1; i < layers.size(); ++i) {
        unsigned layer_size = config.layers[i];
        unsigned prev_size = config.layers[i - 1];
        
        for (unsigned j = 0; j < layer_size; ++j) {
            // Update biases
            float bias_delta = layers[i].errors[j] * config.learning_rate;
            layers[i].biases[j] += bias_delta + layers[i].prev_biases_delta[j] * config.momentum;
            layers[i].prev_biases_delta[j] = bias_delta;
            
            // Update weights
            for (unsigned k = 0; k < prev_size; ++k) {
                float weight_delta = layers[i].errors[j] * layers[i - 1].activations[k] * config.learning_rate;
                layers[i].weights[j][k] += weight_delta + layers[i].prev_weights_delta[j][k] * config.momentum;
                layers[i].prev_weights_delta[j][k] = weight_delta;
            }
        }
    }
    
    applyWeightDecay();
}

void NeuralNetwork::applyWeightDecay() {
    for (size_t i = 1; i < layers.size(); ++i) {
        unsigned layer_size = config.layers[i];
        unsigned prev_size = config.layers[i - 1];
        
        for (unsigned j = 0; j < layer_size; ++j) {
            layers[i].biases[j] *= (1.0f - config.weight_decay);
            for (unsigned k = 0; k < prev_size; ++k) {
                layers[i].weights[j][k] *= (1.0f - config.weight_decay);
            }
        }
    }
}

// Supervised training (single sample)
void NeuralNetwork::trainSupervised(const std::vector<float>& input, const std::vector<float>& target) {
    predict(input);
    backwardPass(target);
    updateWeights();
}

// Add experience for reinforcement learning
void NeuralNetwork::addExperience(const Experience& exp) {
    if (experiences.size() >= config.max_experiences) {
        // Remove oldest experience if at capacity
        experiences.erase(experiences.begin());
    }
    experiences.push_back(exp);
}

// Train from stored experiences (batch learning)
void NeuralNetwork::trainFromExperiences() {
    if (experiences.empty()) return;
    
    // Shuffle experiences
    std::shuffle(experiences.begin(), experiences.end(), rng);
    
    // Train on batches
    unsigned batch_count = std::min(config.batch_size, static_cast<unsigned>(experiences.size()));
    for (unsigned i = 0; i < batch_count; ++i) {
        const auto& exp = experiences[i];
        trainSupervised(exp.state, exp.action);
    }
}

void NeuralNetwork::clearExperiences() {
    experiences.clear();
}

// Reinforcement learning methods
void NeuralNetwork::remember(const std::vector<float>& state, const std::vector<float>& action, 
                           float reward, const std::vector<float>& next_state, bool terminal) {
    Experience exp;
    exp.state = state;
    exp.action = action;
    exp.reward = reward;
    exp.next_state = next_state;
    exp.terminal = terminal;
    addExperience(exp);
}

std::vector<float> NeuralNetwork::getQValues(const std::vector<float>& state) {
    return predict(state);
}

std::vector<float> NeuralNetwork::selectAction(const std::vector<float>& state, float epsilon) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (dist(rng) < epsilon) {
        // Random action
        std::vector<float> random_action(outputSize());
        std::uniform_real_distribution<float> action_dist(-1.0f, 1.0f);
        for (auto& val : random_action) {
            val = action_dist(rng);
        }
        return random_action;
    } else {
        // Greedy action based on Q-values
        return predict(state);
    }
}

// Serialization
std::string NeuralNetwork::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    
    // Save config
    oss << config.layers.size() << " ";
    for (auto layer_size : config.layers) {
        oss << layer_size << " ";
    }
    oss << static_cast<int>(config.hidden_activation) << " ";
    oss << static_cast<int>(config.output_activation) << " ";
    oss << config.learning_rate << " ";
    oss << config.momentum << " ";
    oss << config.weight_decay << " ";
    oss << config.max_experiences << " ";
    oss << config.batch_size << " ";
    oss << config.discount_factor << "\n";
    
    // Save weights and biases
    for (size_t i = 1; i < layers.size(); ++i) {
        // Biases
        for (float bias : layers[i].biases) {
            oss << bias << " ";
        }
        oss << "\n";
        
        // Weights
        for (const auto& weight_row : layers[i].weights) {
            for (float weight : weight_row) {
                oss << weight << " ";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

void NeuralNetwork::fromString(const std::string& data) {
    std::istringstream iss(data);
    
    // Load config
    unsigned layer_count;
    iss >> layer_count;
    config.layers.resize(layer_count);
    for (unsigned& layer_size : config.layers) {
        iss >> layer_size;
    }
    
    int hidden_act, output_act;
    iss >> hidden_act >> output_act;
    config.hidden_activation = static_cast<Activation>(hidden_act);
    config.output_activation = static_cast<Activation>(output_act);
    
    iss >> config.learning_rate >> config.momentum >> config.weight_decay;
    iss >> config.max_experiences >> config.batch_size >> config.discount_factor;
    
    // Reinitialize layers
    layers.resize(config.layers.size());
    for (size_t i = 0; i < config.layers.size(); ++i) {
        unsigned layer_size = config.layers[i];
        layers[i].biases.resize(layer_size, 0.0f);
        layers[i].activations.resize(layer_size, 0.0f);
        layers[i].errors.resize(layer_size, 0.0f);
        layers[i].prev_biases_delta.resize(layer_size, 0.0f);
        
        if (i > 0) {
            unsigned prev_size = config.layers[i - 1];
            layers[i].weights.resize(layer_size, std::vector<float>(prev_size, 0.0f));
            layers[i].prev_weights_delta.resize(layer_size, std::vector<float>(prev_size, 0.0f));
        }
    }
    
    // Load weights and biases
    for (size_t i = 1; i < layers.size(); ++i) {
        // Biases
        for (float& bias : layers[i].biases) {
            iss >> bias;
        }
        
        // Weights
        for (auto& weight_row : layers[i].weights) {
            for (float& weight : weight_row) {
                iss >> weight;
            }
        }
    }
}

// Mutation for evolutionary algorithms
void NeuralNetwork::mutate(std::mt19937& rng, float rate, float scale) {
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    std::normal_distribution<float> noise(0.0f, 1.0f);
    
    for (size_t i = 1; i < layers.size(); ++i) {
        // Mutate biases
        for (float& bias : layers[i].biases) {
            if (unit(rng) < rate) {
                bias += noise(rng) * scale;
            }
        }
        
        // Mutate weights
        for (auto& weight_row : layers[i].weights) {
            for (float& weight : weight_row) {
                if (unit(rng) < rate) {
                    weight += noise(rng) * scale;
                }
            }
        }
    }
}

NeuralNetwork NeuralNetwork::crossover(const NeuralNetwork& other, std::mt19937& rng) const {
    if (config.layers != other.config.layers) {
        throw std::invalid_argument("Networks must have same architecture for crossover");
    }
    
    Config new_config = config;
    NeuralNetwork child(new_config);
    
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);
    
    for (size_t i = 1; i < layers.size(); ++i) {
        // Crossover biases
        for (unsigned j = 0; j < config.layers[i]; ++j) {
            child.layers[i].biases[j] = unit(rng) < 0.5f ? 
                layers[i].biases[j] : other.layers[i].biases[j];
        }
        
        // Crossover weights
        for (unsigned j = 0; j < config.layers[i]; ++j) {
            for (unsigned k = 0; k < config.layers[i - 1]; ++k) {
                child.layers[i].weights[j][k] = unit(rng) < 0.5f ?
                    layers[i].weights[j][k] : other.layers[i].weights[j][k];
            }
        }
    }
    
    return child;
}

void NeuralNetwork::reset() {
    initializeWeights();
    experiences.clear();
}

float NeuralNetwork::getRandomWeight(std::mt19937& rng) const {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(rng);
}

// PilotNetwork implementation
PilotNetwork::PilotNetwork() 
    : NeuralNetwork(Config{
        {10, 16, 16, 4}, // Input: 10 features, Output: 4 actions
        Activation::LEAKY_RELU,
        Activation::TANH,
        0.05f, // learning rate
        0.9f,  // momentum
        0.0001f, // weight decay
        500,   // max experiences
        16,    // batch size
        0.99f  // discount factor
    }) {}

PilotNetwork::Decision PilotNetwork::makeDecision(const std::vector<float>& state) {
    auto output = predict(state);
    Decision decision;
    
    if (output.size() >= 4) {
        decision.aim_x_delta = output[0];
        decision.aim_y_delta = output[1];
        decision.should_fire = output[2] > 0.0f; // Threshold at 0
        decision.lead_factor = (output[3] + 1.0f) * 0.5f; // Map from [-1,1] to [0,1] then to [0,2]
    }
    
    return decision;
}

void PilotNetwork::trainFromExperience(const std::vector<float>& state, const Decision& decision, float reward) {
    // Convert decision to network output format
    std::vector<float> target_output = {
        decision.aim_x_delta,
        decision.aim_y_delta,
        decision.should_fire ? 1.0f : -1.0f,
        decision.lead_factor * 2.0f - 1.0f // Map from [0,2] to [-1,1]
    };
    
    // Add experience with reward
    Experience exp;
    exp.state = state;
    exp.action = target_output;
    exp.reward = reward;
    addExperience(exp);
    
    // Train on this experience
    trainSupervised(state, target_output);
}

// TargetNetwork implementation
TargetNetwork::TargetNetwork() 
    : NeuralNetwork(Config{
        {12, 16, 16, 4}, // Input: 12 features, Output: 4 movement parameters
        Activation::LEAKY_RELU,
        Activation::TANH,
        0.03f, // learning rate
        0.9f,  // momentum
        0.0001f, // weight decay
        500,   // max experiences
        16,    // batch size
        0.99f  // discount factor
    }) {}

TargetNetwork::Movement TargetNetwork::decideMovement(const std::vector<float>& state) {
    auto output = predict(state);
    Movement movement;
    
    if (output.size() >= 4) {
        movement.velocity_x_delta = output[0];
        movement.velocity_y_delta = output[1];
        movement.dodge_factor = (output[2] + 1.0f) * 0.5f; // Map from [-1,1] to [0,1]
        movement.strafe_direction = output[3]; // Already in [-1,1]
    }
    
    return movement;
}

void TargetNetwork::trainFromExperience(const std::vector<float>& state, const Movement& movement, float reward) {
    // Convert movement to network output format
    std::vector<float> target_output = {
        movement.velocity_x_delta,
        movement.velocity_y_delta,
        movement.dodge_factor * 2.0f - 1.0f, // Map from [0,1] to [-1,1]
        movement.strafe_direction
    };
    
    // Add experience with reward
    Experience exp;
    exp.state = state;
    exp.action = target_output;
    exp.reward = reward;
    addExperience(exp);
    
    // Train on this experience
    trainSupervised(state, target_output);
}

} // namespace TunnelStrike