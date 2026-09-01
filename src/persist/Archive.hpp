#pragma once

#include <string>
#include <vector>

#include "../evo/Genome.hpp"
#include "../evo/Pilot.hpp"

namespace TunnelStrike {

	class World;
	class Population;

	class Archive
	{
	public:
		explicit Archive(const std::string &dir = "evo");

		void ensureDir() const;
		void appendGeneration(unsigned generation, float best_fitness, float mean_fitness,
			float diversity, const std::vector<Genome> &pool);
		void snapshotWorld(const World &world, const Population &population, unsigned long long tick);
		bool loadLatestPool(std::vector<Genome> &out, unsigned *generation = nullptr) const;
		void appendPilot(unsigned generation, float best_fitness, float mean_fitness,
			float diversity, const std::vector<Pilot> &pool);
		bool loadLatestPilot(std::vector<Pilot> &out, unsigned *generation = nullptr) const;
		float assessmentBonus(const Genome &g) const;

		const std::string &directory() const { return dir; }

	private:
		struct Assessment {
			bool wildcard = false;
			float bonus = 0.0f;
			std::string genome_line;
		};

		void loadAssessments();

		std::string dir;
		std::vector<Assessment> assessments;
		std::string journalPath() const { return dir + "/generation.jsonl"; }
		std::string snapshotPath() const { return dir + "/snapshot.txt"; }
		std::string poolPath() const { return dir + "/pool.txt"; }
		std::string assessPath() const { return dir + "/assess.txt"; }
		std::string pilotJournalPath() const { return dir + "/pilot.jsonl"; }
		std::string pilotPoolPath() const { return dir + "/pilot.txt"; }
	};

}
