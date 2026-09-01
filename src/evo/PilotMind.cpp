#include "evo/PilotMind.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "World.hpp"
#include "geometry/camera3d.hpp"
#include "persist/Archive.hpp"

namespace TunnelStrike {

	namespace {
		constexpr float PI = 3.14159265358979323846f;
		constexpr float AIM_LIMIT = 70.0f;
		constexpr float SHOT_SPEED = 1000.0f;
		constexpr double TUNNEL = 9.0;
		constexpr double TUNNEL_BOUND = 8.85;

		float rad2deg(float r)
		{
			return r * 180.0f / PI;
		}

		bool inTunnelXY(const Vector3d &p, double pad = TUNNEL)
		{
			return std::abs(p.x) <= pad && std::abs(p.y) <= pad;
		}

		Vector3d clampTunnel(const Vector3d &p, double bound = TUNNEL_BOUND)
		{
			Vector3d c = p;
			c.x = std::clamp(c.x, -bound, bound);
			c.y = std::clamp(c.y, -bound, bound);
			return c;
		}

		double bounceCoord(double pos, double vel, double t, double bound = TUNNEL_BOUND)
		{
			const double raw = pos + vel * t;
			const double period = 4.0 * bound;
			double shifted = std::fmod(raw + bound, period);
			if (shifted < 0.0)
				shifted += period;
			if (shifted <= 2.0 * bound)
				return shifted - bound;
			else
				return 3.0 * bound - shifted;
		}

		Vector3d dirTo(const Vector3d &from, const Vector3d &to)
		{
			Vector3d d = to - from;
			const double n = d.norm();
			if (n < 1e-6)
				return Vector3d(0, 0, 100);
			d *= 100.0 / n;
			return d;
		}

		Vector3d interceptPoint(const Vector3d &muzzle, const Vector3d &pos, const Vector3d &vel, float lead)
		{
			const Vector3d D = pos - muzzle;
			const double dv = D.x * vel.x + D.y * vel.y;
			const double v2 = vel.x * vel.x + vel.y * vel.y;
			const double s2 = SHOT_SPEED * SHOT_SPEED;
			const double denom = s2 - v2;
			double t = 0.0;
			if (denom > 1e-6) {
				const double d2 = D * D;
				const double disc = dv * dv + denom * d2;
				if (disc >= 0.0) {
					t = (dv + std::sqrt(disc)) / denom;
				}
			}
			if (t <= 0.0) {
				t = D.norm() / SHOT_SPEED;
			}

			t *= static_cast<double>(lead);

			const double ix = bounceCoord(pos.x, vel.x, t);
			const double iy = bounceCoord(pos.y, vel.y, t);
			return Vector3d(ix, iy, pos.z);
		}

		double pointRayDist(const Vector3d &origin, const Vector3d &dir, const Vector3d &p)
		{
			const double n = dir.norm();
			if (n < 1e-9)
				return p.distance_to(origin);
			const Vector3d d = dir * (1.0 / n);
			const Vector3d rel = p - origin;
			double t = rel * d;
			if (t < 0.0)
				t = 0.0;
			return p.distance_to(origin + d * t);
		}
	}

	PilotMind::PilotMind(Archive &store)
		: store(store)
		, rng(std::random_device{}())
	{
		std::vector<Pilot> loaded;
		unsigned generation = 0;
		if (store.loadLatestPilot(loaded, &generation))
			pop.seedFrom(loaded, generation);
		current = pop.nextPolicy();
	}

	bool PilotMind::pickTarget(World &world, const Vector3d &muzzle, Vector3d &pos, Vector3d &vel, float &size)
	{
		const auto sights = world.liveTargets();
		if (sights.empty()) {
			have_lock = false;
			return false;
		}

		int lock_i = -1;
		float lock_d = 1e30f;
		if (have_lock) {
			for (int i = 0; i < static_cast<int>(sights.size()); ++i) {
				const float dlock = static_cast<float>(sights[static_cast<size_t>(i)].pos.distance_to(lock_pos));
				if (dlock < lock_d) {
					lock_d = dlock;
					lock_i = i;
				}
			}
			if (lock_d > 16.0f)
				lock_i = -1;
		}

		int best_i = -1;
		float best = -1e30f;
		for (int i = 0; i < static_cast<int>(sights.size()); ++i) {
			const auto &t = sights[static_cast<size_t>(i)];
			const Vector3d rel = t.pos - muzzle;
			const float dist = static_cast<float>(rel.norm());
			if (rel.z < 6.0 || dist < 3.0f || dist > current.max_dist)
				continue;

			const float zclose = 1.0f / (1.0f + static_cast<float>(rel.z) * 0.004f);
			const float aligned = 1.0f / (1.0f + static_cast<float>(std::hypot(rel.x, rel.y)) * 0.12f);
			const float bulky = t.size / 6.0f;
			const float urgency = rel.z < 60.0 ? (60.0f - static_cast<float>(rel.z)) * 0.02f : 0.0f;

			float score = current.prefer_close * zclose
				+ (1.0f - current.prefer_close) * aligned
				+ current.prefer_size * bulky
				+ urgency;
			if (i == lock_i)
				score += current.stick * 2.0f;
			if (score > best) {
				best = score;
				best_i = i;
			}
		}

		if (lock_i >= 0 && current.stick >= 0.40f) {
			const auto &locked = sights[static_cast<size_t>(lock_i)];
			const Vector3d rel = locked.pos - muzzle;
			const float dist = static_cast<float>(rel.norm());
			if (rel.z >= 6.0 && dist >= 3.0f && dist <= current.max_dist * 1.15f)
				best_i = lock_i;
		}

		if (best_i < 0) {
			have_lock = false;
			return false;
		}

		const auto &chosen = sights[static_cast<size_t>(best_i)];
		pos = chosen.pos;
		vel = chosen.vel;
		size = chosen.size;
		lock_pos = chosen.pos;
		have_lock = true;
		return true;
	}

	void PilotMind::endEpisode()
	{
		const float acc = episode_shots > 0
			? static_cast<float>(episode_kills) / static_cast<float>(episode_shots)
			: 0.0f;
		const int wasted = std::max(0, episode_shots - episode_kills);
		float fitness = episode_kills * 10.0f
			+ acc * 8.0f
			- wasted * 0.05f
			- episode_misses * 0.35f
			+ episode_long_range_kills * 1.5f;

		if (episode_had_target > 20 && episode_shots == 0)
			fitness -= 2.0f;

		pop.record(current, fitness);
		if (pop.maybeEvolve()) {
			store.appendPilot(
				pop.generationIndex(),
				pop.lastBestFitness(),
				pop.lastMeanFitness(),
				pop.lastDiversity(),
				pop.pool());
		}

		current = pop.nextPolicy();
		episode_age = 0.0f;
		episode_kills = 0;
		episode_misses = 0;
		episode_shots = 0;
		episode_had_target = 0;
		episode_long_range_kills = 0.0f;
		have_lock = false;
	}

	bool PilotMind::step(sf::Time dt, World &world, float &aim_x, float &aim_y)
	{
		const float seconds = dt.asSeconds();
		clock += seconds;
		episode_age += seconds;
		if (cooldown > 0.0f)
			cooldown -= seconds;

		if (world.get_kills() > seen_kills) {
			const unsigned new_kills = world.get_kills() - seen_kills;
			episode_kills += static_cast<int>(new_kills);
			seen_kills = world.get_kills();
			have_lock = false;
		}
		if (world.get_misses() > seen_misses) {
			episode_misses += static_cast<int>(world.get_misses() - seen_misses);
			seen_misses = world.get_misses();
		}

		if (episode_age >= EPISODE_SECONDS)
			endEpisode();

		const Vector3d muzzle(0, 0, Camera3d::instance().center().get_z());
		Vector3d target;
		Vector3d vel;
		float size = 3.0f;
		const bool have = pickTarget(world, muzzle, target, vel, size);
		if (have)
			++episode_had_target;

		float want_x = 0.0f;
		float want_y = 0.0f;
		Vector3d aim_at = target;
		bool intercept_ok = false;

		if (have) {
			aim_at = interceptPoint(muzzle, target, vel, current.lead);
			intercept_ok = inTunnelXY(aim_at, TUNNEL_BOUND + 0.5);

			const Vector3d rel = aim_at - muzzle;
			const float horiz = static_cast<float>(std::hypot(rel.x, rel.z));
			want_x = rad2deg(static_cast<float>(std::atan2(rel.x, rel.z))) * 4.0f;
			want_y = rad2deg(static_cast<float>(std::atan2(rel.y, std::max(horiz, 0.1f)))) * 4.0f;
		} else {
			want_x = current.yaw_bias * 8.0f
				+ std::sin(clock * (0.8f + current.wander * 1.5f)) * current.wander * 28.0f;
			want_y = current.pitch_bias * 8.0f
				+ std::cos(clock * (0.5f + current.wander * 1.2f)) * current.wander * 18.0f;
		}

		std::uniform_real_distribution<float> unit(0.0f, 1.0f);
		if (!have && unit(rng) < current.jitter) {
			want_x += (unit(rng) - 0.5f) * 8.0f;
			want_y += (unit(rng) - 0.5f) * 8.0f;
		}

		const float slew_rate = have ? (8.0f + current.smooth * 18.0f) : (4.0f + current.smooth * 10.0f);
		const float alpha = std::clamp(1.0f - std::exp(-slew_rate * seconds), 0.05f, 1.0f);
		aim_x = aim_x + (want_x - aim_x) * alpha;
		aim_y = aim_y + (want_y - aim_y) * alpha;
		aim_x = std::clamp(aim_x, -AIM_LIMIT, AIM_LIMIT);
		aim_y = std::clamp(aim_y, -AIM_LIMIT, AIM_LIMIT);

		if (!have || cooldown > 0.0f || !intercept_ok)
			return false;

		Vector3d barrel(0, 0, 100);
		barrel.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), aim_x / 4.0f);
		barrel.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -aim_y / 4.0f);

		const double radius = 1.35 + static_cast<double>(size) * 0.42;
		const double slack = radius * (1.70 - 0.85 * static_cast<double>(current.trigger));
		const double dist_lead = pointRayDist(muzzle, barrel, aim_at);
		const double dist_body = pointRayDist(muzzle, barrel, target);
		const bool on_lead = dist_lead < slack;
		const bool on_body = dist_body < slack * 1.25;
		if (!on_lead && !on_body)
			return false;

		fire_dir = barrel;
		cooldown = current.fire_gap;
		++episode_shots;
		if (target.z - muzzle.z > 150.0)
			episode_long_range_kills += 1.0f;
		return true;
	}

}
