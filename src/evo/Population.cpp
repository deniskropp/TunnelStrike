#include "evo/Population.hpp"

#include <algorithm>

namespace TunnelStrike {

	Population::Population()
		: rng(std::random_device{}())
	{
		refillRandom();
	}

	void Population::refillRandom()
	{
		genomes.clear();
		genomes.reserve(POOL_SIZE);
		for (unsigned i = 0; i < POOL_SIZE; ++i)
			genomes.push_back(Genome::random(rng));
		cursor = 0;
	}

	void Population::seedFrom(const std::vector<Genome> &loaded)
	{
		if (loaded.empty())
			return;

		genomes = loaded;
		while (genomes.size() < POOL_SIZE)
			genomes.push_back(genomes[genomes.size() % loaded.size()].mutated(rng));
		if (genomes.size() > POOL_SIZE)
			genomes.resize(POOL_SIZE);
		cursor = 0;
	}

	Genome Population::nextSpawn()
	{
		if (genomes.empty())
			refillRandom();

		Genome g = genomes[cursor % genomes.size()];
		++cursor;
		return g.mutated(rng, 0.08f);
	}

	void Population::record(const Genome &genome, float fitness)
	{
		scored.emplace_back(genome, fitness);
	}

	bool Population::maybeEvolve()
	{
		if (scored.size() < BATCH_SIZE)
			return false;

		evolveOnce();
		return true;
	}

	void Population::evolveOnce()
	{
		std::sort(scored.begin(), scored.end(),
			[](const auto &a, const auto &b) { return a.second > b.second; });

		last_best = scored.front().second;

		const unsigned elite_n = std::max(2u, POOL_SIZE / 6);
		std::vector<Genome> next;
		next.reserve(POOL_SIZE);

		for (unsigned i = 0; i < elite_n && i < scored.size(); ++i)
			next.push_back(scored[i].first);

		std::uniform_int_distribution<size_t> pick(0, std::min<size_t>(scored.size(), 8) - 1);

		while (next.size() < POOL_SIZE) {
			const Genome &a = scored[pick(rng)].first;
			const Genome &b = scored[pick(rng)].first;
			next.push_back(Genome::crossover(a, b, rng).mutated(rng));
		}

		genomes.swap(next);
		scored.clear();
		cursor = 0;
		++generation;
	}

}
