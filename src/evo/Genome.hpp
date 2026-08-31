#pragma once

#include <algorithm>
#include <cstdint>
#include <random>
#include <sstream>
#include <string>

#include <SFML/Graphics.hpp>

namespace TunnelStrike {

	struct Genome
	{
		float vx = 0.0f;
		float vy = 0.0f;
		float jitter = 0.02f;
		float speed = 80.0f;
		float size = 3.0f;
		float morph_spread = 3.0f;
		int morph_segments = 40;
		float cr = 0.4f;
		float cg = 1.0f;
		float cb = 0.6f;

		static Genome random(std::mt19937 &rng)
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			std::uniform_real_distribution<float> signed_v(-0.5f, 0.5f);
			std::uniform_int_distribution<int> segs(16, 56);

			Genome g;
			g.vx = signed_v(rng);
			g.vy = signed_v(rng);
			g.jitter = 0.005f + unit(rng) * 0.06f;
			g.speed = 40.0f + unit(rng) * 120.0f;
			g.size = 2.0f + unit(rng) * 3.5f;
			g.morph_spread = 1.5f + unit(rng) * 4.0f;
			g.morph_segments = segs(rng);
			g.cr = unit(rng);
			g.cg = 0.4f + unit(rng) * 0.6f;
			g.cb = unit(rng);
			return g;
		}

		Genome mutated(std::mt19937 &rng, float rate = 0.18f) const
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			std::normal_distribution<float> noise(0.0f, 1.0f);

			Genome g = *this;
			auto nudge = [&](float &v, float sigma, float lo, float hi) {
				if (unit(rng) < rate)
					v = std::clamp(v + noise(rng) * sigma, lo, hi);
			};

			nudge(g.vx, 0.12f, -0.8f, 0.8f);
			nudge(g.vy, 0.12f, -0.8f, 0.8f);
			nudge(g.jitter, 0.01f, 0.001f, 0.12f);
			nudge(g.speed, 18.0f, 20.0f, 200.0f);
			nudge(g.size, 0.4f, 1.2f, 7.0f);
			nudge(g.morph_spread, 0.5f, 0.8f, 8.0f);
			nudge(g.cr, 0.12f, 0.0f, 1.0f);
			nudge(g.cg, 0.12f, 0.0f, 1.0f);
			nudge(g.cb, 0.12f, 0.0f, 1.0f);

			if (unit(rng) < rate) {
				g.morph_segments = std::clamp(
					g.morph_segments + static_cast<int>(noise(rng) * 6.0f),
					8, 72);
			}
			return g;
		}

		static Genome crossover(const Genome &a, const Genome &b, std::mt19937 &rng)
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			Genome g;
			g.vx = unit(rng) < 0.5f ? a.vx : b.vx;
			g.vy = unit(rng) < 0.5f ? a.vy : b.vy;
			g.jitter = unit(rng) < 0.5f ? a.jitter : b.jitter;
			g.speed = (a.speed + b.speed) * 0.5f;
			g.size = unit(rng) < 0.5f ? a.size : b.size;
			g.morph_spread = (a.morph_spread + b.morph_spread) * 0.5f;
			g.morph_segments = unit(rng) < 0.5f ? a.morph_segments : b.morph_segments;
			g.cr = (a.cr + b.cr) * 0.5f;
			g.cg = (a.cg + b.cg) * 0.5f;
			g.cb = (a.cb + b.cb) * 0.5f;
			return g;
		}

		sf::Color color() const
		{
			return sf::Color(
				static_cast<std::uint8_t>(std::clamp(cr, 0.0f, 1.0f) * 255.0f),
				static_cast<std::uint8_t>(std::clamp(cg, 0.0f, 1.0f) * 255.0f),
				static_cast<std::uint8_t>(std::clamp(cb, 0.0f, 1.0f) * 255.0f));
		}

		std::string toLine() const
		{
			std::ostringstream os;
			os << vx << ' ' << vy << ' ' << jitter << ' ' << speed << ' '
			   << size << ' ' << morph_spread << ' ' << morph_segments << ' '
			   << cr << ' ' << cg << ' ' << cb;
			return os.str();
		}

		static Genome fromLine(const std::string &line)
		{
			Genome g;
			std::istringstream is(line);
			is >> g.vx >> g.vy >> g.jitter >> g.speed
			   >> g.size >> g.morph_spread >> g.morph_segments
			   >> g.cr >> g.cg >> g.cb;
			return g;
		}
	};

}
