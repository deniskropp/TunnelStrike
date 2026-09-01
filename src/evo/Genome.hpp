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
		int limbs = 5;
		float limb_len = 1.0f;
		float fork = 0.2f;
		float twist = 0.0f;

		int limbCount() const { return std::max(3, std::min(8, limbs)); }

		bool forked() const { return fork > 0.35f; }

		int ribCount() const { return std::clamp(morph_segments / 20, 0, 3); }

		int bodySegmentCount() const
		{
			return limbCount() * (1 + (forked() ? 1 : 0) + ribCount());
		}

		static Genome random(std::mt19937 &rng)
		{
			std::uniform_real_distribution<float> unit(0.0f, 1.0f);
			std::uniform_real_distribution<float> signed_v(-0.5f, 0.5f);
			std::uniform_int_distribution<int> segs(16, 56);
			std::uniform_int_distribution<int> limb_n(3, 8);

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
			g.limbs = limb_n(rng);
			g.limb_len = 0.55f + unit(rng) * 1.35f;
			g.fork = unit(rng);
			g.twist = unit(rng) * 6.28318530718f;
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
			if (unit(rng) < rate) {
				g.limbs = std::clamp(
					g.limbs + static_cast<int>(noise(rng) * 2.0f),
					3, 8);
			}
			nudge(g.limb_len, 0.18f, 0.35f, 2.2f);
			nudge(g.fork, 0.12f, 0.0f, 1.0f);
			nudge(g.twist, 0.35f, 0.0f, 6.28318530718f);
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
			g.limbs = unit(rng) < 0.5f ? a.limbs : b.limbs;
			g.limb_len = (a.limb_len + b.limb_len) * 0.5f;
			g.fork = unit(rng) < 0.5f ? a.fork : b.fork;
			g.twist = (a.twist + b.twist) * 0.5f;
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
			   << cr << ' ' << cg << ' ' << cb << ' '
			   << limbs << ' ' << limb_len << ' ' << fork << ' ' << twist;
			return os.str();
		}

		static Genome fromLine(const std::string &line)
		{
			Genome g;
			std::istringstream is(line);
			is >> g.vx >> g.vy >> g.jitter >> g.speed
			   >> g.size >> g.morph_spread >> g.morph_segments
			   >> g.cr >> g.cg >> g.cb;

			int parsed_limbs = 0;
			float parsed_len = 0.0f;
			float parsed_fork = 0.0f;
			float parsed_twist = 0.0f;
			if (is >> parsed_limbs >> parsed_len >> parsed_fork >> parsed_twist) {
				g.limbs = parsed_limbs;
				g.limb_len = parsed_len;
				g.fork = parsed_fork;
				g.twist = parsed_twist;
			}
			return g;
		}
	};

}
