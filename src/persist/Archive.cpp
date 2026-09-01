#include "persist/Archive.hpp"

#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "../World.hpp"
#include "../Targets.hpp"
#include "../evo/Population.hpp"
#include "../geometry/camera3d.hpp"

namespace TunnelStrike {

	Archive::Archive(const std::string &dir)
		: dir(dir)
	{
		ensureDir();
		loadAssessments();
	}

	void Archive::loadAssessments()
	{
		assessments.clear();
		std::ifstream file(assessPath());
		if (!file)
			return;

		std::string line;
		while (std::getline(file, line)) {
			if (line.empty() || line[0] == '#')
				continue;

			std::istringstream is(line);
			std::string first;
			if (!(is >> first))
				continue;

			if (first == "*") {
				float bonus = 0.0f;
				if (!(is >> bonus))
					continue;
				assessments.push_back({true, bonus, {}});
				continue;
			}

			float bonus = 0.0f;
			try {
				bonus = std::stof(first);
			} catch (...) {
				continue;
			}

			std::string rest;
			std::getline(is, rest);
			if (!rest.empty() && rest[0] == ' ')
				rest.erase(0, 1);
			if (rest.empty())
				continue;

			assessments.push_back({false, bonus, rest});
		}
	}

	float Archive::assessmentBonus(const Genome &g) const
	{
		float bonus = 0.0f;
		const std::string line = g.toLine();
		for (const auto &a : assessments) {
			if (a.wildcard)
				bonus += a.bonus;
			else if (a.genome_line == line)
				bonus += a.bonus;
		}
		return bonus;
	}

	void Archive::ensureDir() const
	{
#if defined(_WIN32)
		_mkdir(dir.c_str());
#else
		::mkdir(dir.c_str(), 0755);
#endif
	}

	void Archive::appendGeneration(unsigned generation, float best_fitness, const std::vector<Genome> &pool)
	{
		ensureDir();
		std::ofstream journal(journalPath(), std::ios::app);
		journal << "{\"generation\":" << generation
			<< ",\"best\":" << best_fitness
			<< ",\"pool\":" << pool.size() << "}\n";

		std::ofstream pool_file(poolPath(), std::ios::trunc);
		for (const auto &g : pool)
			pool_file << g.toLine() << '\n';
	}

	void Archive::snapshotWorld(const World &world, const Population &population, unsigned long long tick)
	{
		ensureDir();
		std::ofstream file(snapshotPath(), std::ios::trunc);

		file << "tick " << tick << '\n';
		file << "generation " << population.generationIndex() << '\n';
		file << "best " << population.lastBestFitness() << '\n';
		file << "kills " << world.get_kills() << '\n';
		file << "camera "
			<< Camera3d::instance().center().get_x() << ' '
			<< Camera3d::instance().center().get_y() << ' '
			<< Camera3d::instance().center().get_z() << '\n';

		if (world.targets) {
			for (const auto &t : world.targets->targets) {
				file << "target "
					<< t->GetCenter().get_x() << ' '
					<< t->GetCenter().get_y() << ' '
					<< t->GetCenter().get_z() << ' '
					<< t->genome().toLine() << '\n';
			}
		}
	}

	bool Archive::loadLatestPool(std::vector<Genome> &out) const
	{
		std::ifstream file(poolPath());
		if (!file)
			return false;

		out.clear();
		std::string line;
		while (std::getline(file, line)) {
			if (line.empty())
				continue;
			out.push_back(Genome::fromLine(line));
		}
		return !out.empty();
	}

}
