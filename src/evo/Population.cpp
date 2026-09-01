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
		spawn_elites = std::min(4u, static_cast<unsigned>(genomes.size()));
	}

	const Genome &Population::tournamentPick(unsigned among)
	{
		among = std::max(1u, std::min(among, static_cast<unsigned>(scored.size())));
		std::uniform_int_distribution<size_t> pick(0, among - 1);
		size_t best_i = pick(rng);
		for (int n = 0; n < 3; ++n) {
			const size_t i = pick(rng);
			const float fi = i < pick_fit.size() ? pick_fit[i] : scored[i].second;
			const float fb = best_i < pick_fit.size() ? pick_fit[best_i] : scored[best_i].second;
			if (fi > fb)
				best_i = i;
		}
		return scored[best_i].first;
	}

	Genome Population::nextSpawn()
	{
		if (genomes.empty())
			refillRandom();

		std::uniform_real_distribution<float> unit(0.0f, 1.0f);
		if (unit(rng) < 0.58f && spawn_elites > 0) {
			std::uniform_int_distribution<unsigned> elite(0, spawn_elites - 1);
			return genomes[elite(rng) % genomes.size()].mutated(rng, 0.06f);
		}

		Genome g = genomes[cursor % genomes.size()];
		++cursor;
		return g.mutated(rng, 0.10f);
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

		pick_fit.resize(scored.size());
		for (size_t i = 0; i < scored.size(); ++i) {
			float share = 1.0f;
			for (size_t j = 0; j < i; ++j) {
				if (scored[i].first.geneDistance(scored[j].first) < 0.4f)
					share *= 0.7f;
			}
			pick_fit[i] = scored[i].second * share;
		}

		const float mut = 0.11f + 0.20f / (1.0f + last_diversity * 4.0f);
		const unsigned immigrants = last_diversity < 0.22f ? 4u : 2u;
		const unsigned parent_n = std::min<unsigned>(
			static_cast<unsigned>(scored.size()),
			std::max(8u, static_cast<unsigned>(scored.size() * 3 / 4)));

		std::vector<Genome> next;
		next.reserve(POOL_SIZE);

		next.push_back(scored[0].first);
		if (scored.size() > 1)
			next.push_back(scored[1].first);
		next.push_back(scored[0].first.mutated(rng, mut * 0.45f));
		if (scored.size() > 1)
			next.push_back(scored[1].first.mutated(rng, mut * 0.45f));
		spawn_elites = std::min(4u, static_cast<unsigned>(next.size()));

		while (next.size() + immigrants < POOL_SIZE) {
			const Genome &a = tournamentPick(parent_n);
			const Genome &b = tournamentPick(parent_n);
			next.push_back(Genome::crossover(a, b, rng).mutated(rng, mut));
		}

		while (next.size() < POOL_SIZE)
			next.push_back(Genome::random(rng));

		float mean_speed = 0.0f;
		float mean_seek = 0.0f;
		float mean_dodge = 0.0f;
		float mean_strafe = 0.0f;
		float mean_range = 0.0f;
		for (const auto &g : next) {
			mean_speed += g.speed;
			mean_seek += g.seek;
			mean_dodge += g.dodge;
			mean_strafe += g.strafe;
			mean_range += g.range;
		}
		const float n = static_cast<float>(next.size());
		mean_speed /= n;
		mean_seek /= n;
		mean_dodge /= n;
		mean_strafe /= n;
		mean_range /= n;
		float var = 0.0f;
		for (const auto &g : next) {
			const float ds = (g.speed - mean_speed) * 0.01f;
			const float dk = g.seek - mean_seek;
			const float dd = g.dodge - mean_dodge;
			const float df = g.strafe - mean_strafe;
			const float dr = (g.range - mean_range) * 0.12f;
			var += ds * ds + dk * dk + dd * dd + df * df + dr * dr;
		}
		last_diversity = std::sqrt(var / n);

		genomes.swap(next);
		scored.clear();
		cursor = 0;
		++generation;
	}

}
