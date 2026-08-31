#include "persist/Archive.hpp"

#include <fstream>

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
