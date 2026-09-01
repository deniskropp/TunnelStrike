#include "evo/Population.hpp"

#include <algorithm>
#include <cmath>

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

	void Population::seedFrom(const std::vector<Genome> &loaded, unsigned generation_index)
	{
		if (loaded.empty())
			return;

		genomes = loaded;
		while (genomes.size() < POOL_SIZE)
			genomes.push_back(genomes[genomes.size() % loaded.size()].mutated(rng));
		if (genomes.size() > POOL_SIZE)
			genomes.resize(POOL_SIZE);
		cursor = 0;
		generation = generation_index;
	}

	const Genome &Population::tournamentPick()
	{
		std::uniform_int_distribution<size_t> pick(0, scored.size() - 1);
		const size_t a = pick(rng);
		const size_t b = pick(rng);
		const size_t c = pick(rng);
		const auto *best = &scored[a];
		if (scored[b].second > best->second)
			best = &scored[b];
		if (scored[c].second > best->second)
			best = &scored[c];
		return best->first;
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
		float sum = 0.0f;
		for (const auto &s : scored)
			sum += s.second;
		last_mean = sum / static_cast<float>(scored.size());

		const unsigned elite_n = std::max(2u, POOL_SIZE / 6);
		const unsigned immigrants = 2;
		std::vector<Genome> next;
		next.reserve(POOL_SIZE);

		for (unsigned i = 0; i < elite_n && i < scored.size(); ++i)
			next.push_back(scored[i].first);

		while (next.size() + immigrants < POOL_SIZE) {
			const Genome &a = tournamentPick();
			const Genome &b = tournamentPick();
			next.push_back(Genome::crossover(a, b, rng).mutated(rng));
		}

		while (next.size() < POOL_SIZE)
			next.push_back(Genome::random(rng));

		float mean_speed = 0.0f;
		float mean_seek = 0.0f;
		float mean_dodge = 0.0f;
		for (const auto &g : next) {
			mean_speed += g.speed;
			mean_seek += g.seek;
			mean_dodge += g.dodge;
		}
		const float n = static_cast<float>(next.size());
		mean_speed /= n;
		mean_seek /= n;
		mean_dodge /= n;
		float var = 0.0f;
		for (const auto &g : next) {
			const float ds = (g.speed - mean_speed) * 0.01f;
			const float dk = g.seek - mean_seek;
			const float dd = g.dodge - mean_dodge;
			var += ds * ds + dk * dk + dd * dd;
		}
		last_diversity = std::sqrt(var / n);

		genomes.swap(next);
		scored.clear();
		cursor = 0;
		++generation;
	}

}
