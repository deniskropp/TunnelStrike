#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace TunnelStrike {

	struct Pilot
	{
		float lead = 1.0f;
		float smooth = 0.55f;
		float jitter = 0.01f;
		float fire_gap = 0.26f;
		float max_dist = 560.0f;
		float prefer_close = 0.65f;
		float prefer_size = 0.2f;
		float wander = 0.08f;
		float trigger = 0.5f;
		float yaw_bias = 0.0f;
		float pitch_bias = 0.0f;
		float stick = 0.7f;

		void clamp()
		{
			lead = std::clamp(lead, 0.2f, 1.8f);
			smooth = std::clamp(smooth, 0.2f, 1.0f);
			jitter = std::clamp(jitter, 0.0f, 0.08f);
			fire_gap = std::clamp(fire_gap, 0.15f, 0.7f);
			max_dist = std::clamp(max_dist, 120.0f, 780.0f);
			prefer_close = std::clamp(prefer_close, 0.0f, 1.0f);
			prefer_size = std::clamp(prefer_size, 0.0f, 1.0f);
			wander = std::clamp(wander, 0.0f, 0.5f);
			trigger = std::clamp(trigger, 0.2f, 0.9f);
			yaw_bias = std::clamp(yaw_bias, -0.5f, 0.5f);
			pitch_bias = std::clamp(pitch_bias, -0.5f, 0.5f);
			stick = std::clamp(stick, 0.2f, 0.95f);
		}

		float geneDistance(const Pilot &o) const
		{
			auto d = [](float a, float b) { return std::abs(a - b); };
			return d(lead, o.lead) + d(smooth, o.smooth) + d(trigger, o.trigger)
				+ d(prefer_close, o.prefer_close) + d(fire_gap, o.fire_gap)
				+ d(max_dist, o.max_dist) * 0.002f + d(wander, o.wander)
				+ d(stick, o.stick);
		}

		static Pilot random(std::mt19937 &rng)
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			std::uniform_real_distribution<float> signed_v(-0.5f, 0.5f);
			Pilot p;
			p.lead = 0.7f + unit(rng) * 0.7f;
			p.smooth = 0.4f + unit(rng) * 0.5f;
			p.jitter = unit(rng) * 0.04f;
			p.fire_gap = 0.18f + unit(rng) * 0.28f;
			p.max_dist = 360.0f + unit(rng) * 320.0f;
			p.prefer_close = 0.4f + unit(rng) * 0.5f;
			p.prefer_size = unit(rng) * 0.45f;
			p.wander = unit(rng) * 0.22f;
			p.trigger = 0.35f + unit(rng) * 0.4f;
			p.yaw_bias = signed_v(rng) * 0.4f;
			p.pitch_bias = signed_v(rng) * 0.4f;
			p.stick = 0.5f + unit(rng) * 0.4f;
			p.clamp();
			return p;
		}

		Pilot mutated(std::mt19937 &rng, float rate = 0.2f) const
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			std::normal_distribution<float> noise(0.0f, 1.0f);
			Pilot p = *this;
			auto nudge = [&](float &v, float sigma, float lo, float hi) {
				if (unit(rng) < rate)
					v = std::clamp(v + noise(rng) * sigma, lo, hi);
			};
			nudge(p.lead, 0.12f, 0.2f, 1.8f);
			nudge(p.smooth, 0.08f, 0.2f, 1.0f);
			nudge(p.jitter, 0.01f, 0.0f, 0.08f);
			nudge(p.fire_gap, 0.05f, 0.15f, 0.7f);
			nudge(p.max_dist, 40.0f, 120.0f, 780.0f);
			nudge(p.prefer_close, 0.1f, 0.0f, 1.0f);
			nudge(p.prefer_size, 0.1f, 0.0f, 1.0f);
			nudge(p.wander, 0.05f, 0.0f, 0.5f);
			nudge(p.trigger, 0.08f, 0.2f, 0.9f);
			nudge(p.yaw_bias, 0.06f, -0.5f, 0.5f);
			nudge(p.pitch_bias, 0.06f, -0.5f, 0.5f);
			nudge(p.stick, 0.08f, 0.2f, 0.95f);
			p.clamp();
			return p;
		}

		static Pilot crossover(const Pilot &a, const Pilot &b, std::mt19937 &rng)
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			Pilot p;
			p.lead = (a.lead + b.lead) * 0.5f;
			p.smooth = unit(rng) < 0.5f ? a.smooth : b.smooth;
			p.jitter = unit(rng) < 0.5f ? a.jitter : b.jitter;
			p.fire_gap = (a.fire_gap + b.fire_gap) * 0.5f;
			p.max_dist = unit(rng) < 0.5f ? a.max_dist : b.max_dist;
			p.prefer_close = (a.prefer_close + b.prefer_close) * 0.5f;
			p.prefer_size = unit(rng) < 0.5f ? a.prefer_size : b.prefer_size;
			p.wander = unit(rng) < 0.5f ? a.wander : b.wander;
			p.trigger = (a.trigger + b.trigger) * 0.5f;
			p.yaw_bias = unit(rng) < 0.5f ? a.yaw_bias : b.yaw_bias;
			p.pitch_bias = unit(rng) < 0.5f ? a.pitch_bias : b.pitch_bias;
			p.stick = (a.stick + b.stick) * 0.5f;
			p.clamp();
			return p;
		}

		std::string toLine() const
		{
			std::ostringstream os;
			os << lead << ' ' << smooth << ' ' << jitter << ' ' << fire_gap << ' '
			   << max_dist << ' ' << prefer_close << ' ' << prefer_size << ' '
			   << wander << ' ' << trigger << ' ' << yaw_bias << ' ' << pitch_bias << ' '
			   << stick;
			return os.str();
		}

		static Pilot fromLine(const std::string &line)
		{
			Pilot p;
			std::istringstream is(line);
			is >> p.lead >> p.smooth >> p.jitter >> p.fire_gap
			   >> p.max_dist >> p.prefer_close >> p.prefer_size
			   >> p.wander >> p.trigger >> p.yaw_bias >> p.pitch_bias;
			float parsed_stick = 0.0f;
			if (is >> parsed_stick)
				p.stick = parsed_stick;
			p.clamp();
			return p;
		}
	};

	class PilotPop
	{
	public:
		static constexpr unsigned POOL_SIZE = 16;
		static constexpr unsigned BATCH_SIZE = 8;

		PilotPop()
			: rng(std::random_device{}())
		{
			refill();
		}

		Pilot nextPolicy()
		{
			if (genomes.empty())
				refill();
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			if (unit(rng) < 0.6f && !genomes.empty()) {
				std::uniform_int_distribution<unsigned> elite(0, std::min(3u, static_cast<unsigned>(genomes.size()) - 1));
				return genomes[elite(rng)].mutated(rng, 0.07f);
			}
			Pilot g = genomes[cursor % genomes.size()];
			++cursor;
			return g.mutated(rng, 0.12f);
		}

		void record(const Pilot &pilot, float fitness)
		{
			scored.emplace_back(pilot, fitness);
		}

		bool maybeEvolve()
		{
			if (scored.size() < BATCH_SIZE)
				return false;
			evolveOnce();
			return true;
		}

		void seedFrom(const std::vector<Pilot> &loaded, unsigned generation_index = 0)
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

		unsigned generationIndex() const { return generation; }
		unsigned scoredPending() const { return static_cast<unsigned>(scored.size()); }
		const std::vector<Pilot> &pool() const { return genomes; }
		float lastBestFitness() const { return last_best; }
		float lastMeanFitness() const { return last_mean; }
		float lastDiversity() const { return last_diversity; }

	private:
		void refill()
		{
			genomes.clear();
			for (unsigned i = 0; i < POOL_SIZE; ++i)
				genomes.push_back(Pilot::random(rng));
			cursor = 0;
		}

		const Pilot &tournamentPick(unsigned among)
		{
			among = std::max(1u, std::min(among, static_cast<unsigned>(scored.size())));
			std::uniform_int_distribution<size_t> pick(0, among - 1);
			size_t best_i = pick(rng);
			for (int n = 0; n < 3; ++n) {
				const size_t i = pick(rng);
				if (scored[i].second > scored[best_i].second)
					best_i = i;
			}
			return scored[best_i].first;
		}

		void evolveOnce()
		{
			std::sort(scored.begin(), scored.end(),
				[](const auto &a, const auto &b) { return a.second > b.second; });

			last_best = scored.front().second;
			float sum = 0.0f;
			for (const auto &s : scored)
				sum += s.second;
			last_mean = sum / static_cast<float>(scored.size());

			std::vector<Pilot> next;
			next.reserve(POOL_SIZE);
			next.push_back(scored[0].first);
			if (scored.size() > 1)
				next.push_back(scored[1].first);
			next.push_back(scored[0].first.mutated(rng, 0.08f));

			const unsigned parent_n = std::min<unsigned>(
				static_cast<unsigned>(scored.size()),
				std::max(4u, static_cast<unsigned>(scored.size() * 3 / 4)));
			const unsigned immigrants = last_diversity < 0.2f ? 3u : 2u;

			while (next.size() + immigrants < POOL_SIZE) {
				const Pilot &a = tournamentPick(parent_n);
				const Pilot &b = tournamentPick(parent_n);
				next.push_back(Pilot::crossover(a, b, rng).mutated(rng));
			}
			while (next.size() < POOL_SIZE)
				next.push_back(Pilot::random(rng));

			float mean_lead = 0.0f;
			float mean_trig = 0.0f;
			for (const auto &g : next) {
				mean_lead += g.lead;
				mean_trig += g.trigger;
			}
			const float n = static_cast<float>(next.size());
			mean_lead /= n;
			mean_trig /= n;
			float var = 0.0f;
			for (const auto &g : next) {
				const float dl = g.lead - mean_lead;
				const float dt = g.trigger - mean_trig;
				var += dl * dl + dt * dt;
			}
			last_diversity = std::sqrt(var / n);

			genomes.swap(next);
			scored.clear();
			cursor = 0;
			++generation;
		}

		std::vector<Pilot> genomes;
		std::vector<std::pair<Pilot, float>> scored;
		unsigned generation = 0;
		unsigned cursor = 0;
		float last_best = 0.0f;
		float last_mean = 0.0f;
		float last_diversity = 0.0f;
		std::mt19937 rng;
	};

}
