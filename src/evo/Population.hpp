#pragma once

#include <utility>
#include <vector>
#include <random>

#include "Genome.hpp"

namespace TunnelStrike {

	class Population
	{
	public:
		static constexpr unsigned POOL_SIZE = 24;
		static constexpr unsigned BATCH_SIZE = 16;
		static constexpr unsigned MAX_GENERATIONS_PER_STEP = 1;

		Population();

		Genome nextSpawn();
		void record(const Genome &genome, float fitness);
		bool maybeEvolve();

		unsigned generationIndex() const { return generation; }
		unsigned scoredPending() const { return static_cast<unsigned>(scored.size()); }
		const std::vector<Genome> &pool() const { return genomes; }
		float lastBestFitness() const { return last_best; }
		float lastMeanFitness() const { return last_mean; }
		float lastDiversity() const { return last_diversity; }

		void seedFrom(const std::vector<Genome> &loaded, unsigned generation_index = 0);

	private:
		void refillRandom();
		void evolveOnce();
		const Genome &tournamentPick();

		std::vector<Genome> genomes;
		std::vector<std::pair<Genome, float>> scored;
		unsigned generation = 0;
		unsigned cursor = 0;
		float last_best = 0.0f;
		float last_mean = 0.0f;
		float last_diversity = 0.0f;
		std::mt19937 rng;
	};

}
