#pragma once

#include <vector>
#include <random>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <cmath>

namespace TunnelStrike {

class NeuralNetwork {
public:
    // Activation functions
    enum class Activation {
        SIGMOID,
        TANH,
        RELU,
        LEAKY_RELU,
        LINEAR
    };

    // Network configuration
    struct Config {
        std::vector<unsigned> layers; // Number of neurons in each layer (including input and output)
        Activation hidden_activation = Activation::LEAKY_RELU;
        Activation output_activation = Activation::TANH;
        float learning_rate = 0.01f;
        float momentum = 0.9f;
        float weight_decay = 0.0001f;
        unsigned max_experiences = 1000;
        unsigned batch_size = 32;
        float discount_factor = 0.99f; // For reinforcement learning
    };

    // Experience for reinforcement learning
    struct Experience {
        std::vector<float> state;
        std::vector<float> action;
        float reward = 0.0f;
        std::vector<float> next_state;
        bool terminal = false;
    };

public:
    NeuralNetwork(const Config& config);
    NeuralNetwork(const NeuralNetwork& other);
    NeuralNetwork& operator=(const NeuralNetwork& other);
    ~NeuralNetwork();

    // Forward pass
    std::vector<float> predict(const std::vector<float>& input);
    float predictSingle(const std::vector<float>& input);

    // Training methods
    void trainSupervised(const std::vector<float>& input, const std::vector<float>& target);
    void addExperience(const Experience& exp);
    void trainFromExperiences();
    void clearExperiences();

    // Reinforcement learning
    void remember(const std::vector<float>& state, const std::vector<float>& action, 
                  float reward, const std::vector<float>& next_state, bool terminal);
    std::vector<float> getQValues(const std::vector<float>& state);
    std::vector<float> selectAction(const std::vector<float>& state, float epsilon = 0.1f);

    // Network info
    unsigned inputSize() const { return config.layers.front(); }
    unsigned outputSize() const { return config.layers.back(); }
    const Config& getConfig() const { return config; }

    // Serialization
    std::string toString() const;
    void fromString(const std::string& data);

    // Mutation for evolutionary algorithms
    void mutate(std::mt19937& rng, float rate = 0.1f, float scale = 0.2f);
    NeuralNetwork crossover(const NeuralNetwork& other, std::mt19937& rng) const;

    // Utility
    void reset();
    float getRandomWeight(std::mt19937& rng) const;

private:
    struct Layer {
        std::vector<std::vector<float>> weights; // [neuron][prev_neuron]
        std::vector<float> biases;
        std::vector<float> activations;
        std::vector<float> errors;
        std::vector<std::vector<float>> prev_weights_delta; // For momentum
        std::vector<float> prev_biases_delta;
    };

    Config config;
    std::vector<Layer> layers;
    std::vector<Experience> experiences;
    mutable std::mt19937 rng;

    // Activation functions
    static float activate(float x, Activation func);
    static float activateDerivative(float x, Activation func);

    // Network operations
    void initializeWeights();
    void forwardPass(const std::vector<float>& input) const;
    void backwardPass(const std::vector<float>& target);
    void updateWeights();

    // Helper methods
    float randomWeight() const;
    void applyWeightDecay();
};

// Neural network for pilot decision making
class PilotNetwork : public NeuralNetwork {
public:
    // Input: [target_distance, target_angle_x, target_angle_y, target_velocity_x, target_velocity_y, 
    //         target_size, player_velocity_x, player_velocity_y, has_target, cooldown_remaining]
    // Output: [aim_x_delta, aim_y_delta, should_fire, lead_factor]
    PilotNetwork();
    
    struct Decision {
        float aim_x_delta = 0.0f;
        float aim_y_delta = 0.0f;
        bool should_fire = false;
        float lead_factor = 1.0f;
    };

    Decision makeDecision(const std::vector<float>& state);
    void trainFromExperience(const std::vector<float>& state, const Decision& decision, float reward);
};

// Neural network for target movement and dodging
class TargetNetwork : public NeuralNetwork {
public:
    // Input: [player_distance, player_angle, player_velocity_x, player_velocity_y,
    //         shot_distance, shot_angle, shot_velocity_x, shot_velocity_y, 
    //         current_velocity_x, current_velocity_y, near_wall, health]
    // Output: [velocity_x_delta, velocity_y_delta, dodge_factor, strafe_direction]
    TargetNetwork();
    
    struct Movement {
        float velocity_x_delta = 0.0f;
        float velocity_y_delta = 0.0f;
        float dodge_factor = 0.0f;
        float strafe_direction = 0.0f; // -1 to 1
    };

    Movement decideMovement(const std::vector<float>& state);
    void trainFromExperience(const std::vector<float>& state, const Movement& movement, float reward);
};

} // namespace TunnelStrike