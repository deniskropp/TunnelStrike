#pragma once

#include <random>

#include <SFML/System.hpp>

#include "Pilot.hpp"
#include "NeuralNetwork.hpp"
#include "../geometry/vector3d.hpp"

namespace TunnelStrike {

class Archive;
class World;

class PilotMind
{
public:
	static constexpr float EPISODE_SECONDS = 9.0f;
	static constexpr bool USE_NEURAL_NETWORK = true;
	static constexpr float NEURAL_EPSILON = 0.15f;
	static constexpr float NEURAL_LEARNING_RATE = 0.05f;

	explicit PilotMind(Archive &store);

	bool step(sf::Time dt, World &world, float &aim_x, float &aim_y);

	Vector3d fireDir() const { return fire_dir; }

	unsigned generation() const { return pop.generationIndex(); }
	unsigned scoredPending() const { return pop.scoredPending(); }
	float lastBestFitness() const { return pop.lastBestFitness(); }
	float lastMeanFitness() const { return pop.lastMeanFitness(); }
	float lastDiversity() const { return pop.lastDiversity(); }
	const Pilot &policy() const { return current; }
	const PilotPop &population() const { return pop; }
	PilotNetwork& neuralPilot() { return pilot_network; }
	const PilotNetwork& neuralPilot() const { return pilot_network; }

private:
	void endEpisode();
	bool pickTarget(World &world, const Vector3d &muzzle, Vector3d &pos, Vector3d &vel, float &size);
	std::vector<float> buildNeuralState(const World &world, const Vector3d &muzzle, bool has_target, 
	                                   const Vector3d &target_pos, const Vector3d &target_vel, float target_size);
	void trainNeuralNetworks(float reward);

	Archive &store;
	PilotPop pop;
	Pilot current;
	PilotNetwork pilot_network;
	float cooldown = 0.0f;
	float episode_age = 0.0f;
	int episode_kills = 0;
	int episode_misses = 0;
	int episode_shots = 0;
	int episode_had_target = 0;
	float episode_long_range_kills = 0.0f;
	unsigned seen_kills = 0;
	unsigned seen_misses = 0;
	Vector3d lock_pos;
	Vector3d fire_dir;
	bool have_lock = false;
	float clock = 0.0f;
	std::mt19937 rng;
	
	// Neural network state tracking
	std::vector<float> last_neural_state;
	PilotNetwork::Decision last_neural_decision;
	bool use_neural_this_step = false;
};

}