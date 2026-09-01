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

		float rad2deg(float r)
		{
			return r * 180.0f / PI;
		}

		Vector3d dirFromAim(float ax, float ay)
		{
			Vector3d dir(0, 0, 100);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), ax / 4.0f);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -ay / 4.0f);
			return dir;
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
			if (lock_d > 14.0f)
				lock_i = -1;
		}

		int best_i = -1;
		float best = -1e30f;
		for (int i = 0; i < static_cast<int>(sights.size()); ++i) {
			const auto &t = sights[static_cast<size_t>(i)];
			const Vector3d rel = t.pos - muzzle;
			const float dist = static_cast<float>(rel.norm());
			if (rel.z < 8.0 || dist < 4.0f || dist > current.max_dist)
				continue;

			const float zclose = 1.0f / (1.0f + static_cast<float>(rel.z) * 0.004f);
			const float aligned = 1.0f / (1.0f + static_cast<float>(std::abs(rel.x) + std::abs(rel.y)) * 0.12f);
			const float bulky = t.size / 7.0f;
			float score = current.prefer_close * zclose
				+ (1.0f - current.prefer_close) * aligned
				+ current.prefer_size * bulky;
			if (i == lock_i)
				score += current.stick * 1.8f;
			if (score > best) {
				best = score;
				best_i = i;
			}
		}

		if (lock_i >= 0 && current.stick >= 0.45f) {
			const auto &locked = sights[static_cast<size_t>(lock_i)];
			const Vector3d rel = locked.pos - muzzle;
			const float dist = static_cast<float>(rel.norm());
			if (rel.z >= 8.0 && dist >= 4.0f && dist <= current.max_dist * 1.1f)
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
		float fitness = episode_kills * 10.0f + acc * 8.0f - wasted * 0.08f - episode_misses * 0.2f;
		if (episode_had_target && episode_shots == 0)
			fitness -= 1.5f;

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
			episode_kills += static_cast<int>(world.get_kills() - seen_kills);
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

		if (have) {
			const float dist = static_cast<float>((target - muzzle).norm());
			const float t_hit = dist / SHOT_SPEED;
			aim_at = target + vel * (t_hit * current.lead);

			const Vector3d rel = aim_at - muzzle;
			const float horiz = static_cast<float>(std::sqrt(rel.x * rel.x + rel.z * rel.z));
			want_x = rad2deg(static_cast<float>(std::atan2(rel.x, rel.z))) * 4.0f;
			want_y = rad2deg(static_cast<float>(std::atan2(rel.y, std::max(horiz, 0.1f)))) * 4.0f;
		} else {
			want_x = current.yaw_bias * 8.0f
				+ std::sin(clock * (0.7f + current.wander)) * current.wander * 22.0f;
			want_y = current.pitch_bias * 8.0f
				+ std::cos(clock * (0.45f + current.wander * 0.6f)) * current.wander * 14.0f;
		}

		std::uniform_real_distribution<float> unit(0.0f, 1.0f);
		const bool settling = have && current.jitter > 0.0f && unit(rng) < current.jitter * 0.35f;
		if (!have && unit(rng) < current.jitter) {
			want_x += (unit(rng) - 0.5f) * 8.0f;
			want_y += (unit(rng) - 0.5f) * 8.0f;
		} else if (settling) {
			want_x += (unit(rng) - 0.5f) * 2.0f;
			want_y += (unit(rng) - 0.5f) * 2.0f;
		}

		const float a = have ? std::max(current.smooth, 0.35f) : current.smooth;
		aim_x = aim_x * (1.0f - a) + want_x * a;
		aim_y = aim_y * (1.0f - a) + want_y * a;
		aim_x = std::clamp(aim_x, -AIM_LIMIT, AIM_LIMIT);
		aim_y = std::clamp(aim_y, -AIM_LIMIT, AIM_LIMIT);

		if (!have || cooldown > 0.0f)
			return false;

		const Vector3d shot_dir = dirFromAim(aim_x, aim_y);
		const double miss = pointRayDist(muzzle, shot_dir, aim_at);
		const double radius = 1.35 + static_cast<double>(size) * 0.42;
		const double slack = radius * (1.85 - 0.7 * static_cast<double>(current.trigger));
		const bool fire = miss < slack;

		if (fire) {
			cooldown = current.fire_gap;
			++episode_shots;
		}

		return fire;
	}

}
