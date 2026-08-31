#pragma once

#include <string>
#include <vector>

#include "../evo/Genome.hpp"

namespace TunnelStrike {

	class World;
	class Population;

	class Archive
	{
	public:
		explicit Archive(const std::string &dir = "evo");

		void ensureDir() const;
		void appendGeneration(unsigned generation, float best_fitness, const std::vector<Genome> &pool);
		void snapshotWorld(const World &world, const Population &population, unsigned long long tick);
		bool loadLatestPool(std::vector<Genome> &out) const;

		const std::string &directory() const { return dir; }

	private:
		std::string dir;
		std::string journalPath() const { return dir + "/generation.jsonl"; }
		std::string snapshotPath() const { return dir + "/snapshot.txt"; }
		std::string poolPath() const { return dir + "/pool.txt"; }
	};

}
