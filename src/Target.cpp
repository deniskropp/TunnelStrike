#include "Target.hpp"

#include <algorithm>
#include <cmath>
#include <functional>


namespace TunnelStrike {

Target::Target(sf::Vector3f spawn, const Genome &genome)
	:
	center(spawn.x, spawn.y, spawn.z),
	size(genome.size),
	color(genome.color()),
	genome_(genome),
	rng_(std::hash<std::string>{}(genome.toLine()))
{
	direction = Vector3d(genome_.vx, genome_.vy, 0.0);
	generate();
}

void Target::generate()
{
	segments.clear();

	const int n = genome_.limbCount();
	const float len = genome_.size * std::max(0.35f, genome_.limb_len);
	const bool forked = genome_.forked();
	const int ribs = genome_.ribCount();
	const float two_pi = 6.28318530718f;

	for (int i = 0; i < n; ++i) {
		const float ang = genome_.twist + two_pi * static_cast<float>(i) / static_cast<float>(n);
		const float ca = std::cos(ang);
		const float sa = std::sin(ang);
		const Vector3d tip(center.x + ca * len, center.y + sa * len, center.z);
		segments.push_back(Segment3d(center, tip, color, color));

		if (forked) {
			const float fang = ang + genome_.fork * 1.2f;
			const Vector3d ftip(
				tip.x + std::cos(fang) * len * 0.45,
				tip.y + std::sin(fang) * len * 0.45,
				center.z);
			segments.push_back(Segment3d(tip, ftip, color, color));
		}

		for (int r = 1; r <= ribs; ++r) {
			const float t = static_cast<float>(r) / static_cast<float>(ribs + 1);
			const Vector3d along(
				center.x + (tip.x - center.x) * t,
				center.y + (tip.y - center.y) * t,
				center.z);
			const float pang = ang + 1.57079632679f;
			const float off = genome_.morph_spread * 0.35f * ((r % 2) ? 1.0f : -1.0f);
			const Vector3d rib(
				along.x + std::cos(pang) * off,
				along.y + std::sin(pang) * off,
				center.z);
			segments.push_back(Segment3d(along, rib, color, color));
		}
	}
}

void Target::Act(sf::Time delta, const Sense &sense)
{
	const float dt = delta.asSeconds();
	lived += dt;

	std::uniform_real_distribution<float> unit(0.0f, 1.0f);
	if (unit(rng_) < genome_.jitter) {
		direction.x += (unit(rng_) - 0.5f) * 0.4f;
		direction.y += (unit(rng_) - 0.5f) * 0.4f;
	}

	Vector3d desired(genome_.vx, genome_.vy, 0.0);

	Vector3d to_player(sense.player.x - center.x, sense.player.y - center.y, 0.0);
	const float player_dist = static_cast<float>(to_player.norm());
	if (player_dist > 1e-4f) {
		to_player.normalize();
		desired.x += to_player.x * genome_.seek;
		desired.y += to_player.y * genome_.seek;
		const float spring = std::clamp((player_dist - genome_.range) / 8.0f, -1.0f, 1.0f);
		desired.x += to_player.x * spring * 0.85f;
		desired.y += to_player.y * spring * 0.85f;
	}
	if (player_dist < 7.0f)
		threat += dt;

	if (sense.has_shot && sense.nearest_shot_dist < 18.0f) {
		Vector3d shot_dir = sense.nearest_shot_dir;
		if (shot_dir.norm() > 1e-6)
			shot_dir.normalize();

		const Vector3d predicted(
			sense.nearest_shot.x + shot_dir.x * genome_.lead * 8.0,
			sense.nearest_shot.y + shot_dir.y * genome_.lead * 8.0,
			sense.nearest_shot.z + shot_dir.z * genome_.lead * 8.0);

		Vector3d away(center.x - predicted.x, center.y - predicted.y, 0.0);
		if (away.norm() < 1e-4) {
			away.x = center.x - sense.nearest_shot.x;
			away.y = center.y - sense.nearest_shot.y;
		}

		const bool react = sense.shot_closing || sense.nearest_shot_dist < 8.0f;
		if (react && away.norm() > 1e-4) {
			away.normalize();
			const float urgency = 1.0f + 6.0f / (sense.nearest_shot_dist + 1.0f);
			desired.x += away.x * genome_.dodge * urgency;
			desired.y += away.y * genome_.dodge * urgency;

			Vector3d perp(-away.y, away.x, 0.0);
			if (std::sin(genome_.twist) < 0.0)
				perp = Vector3d(-perp.x, -perp.y, 0.0);
			if (perp.norm() > 1e-4) {
				perp.normalize();
				desired.x += perp.x * genome_.strafe * urgency;
				desired.y += perp.y * genome_.strafe * urgency;
			}
		}

		if (sense.nearest_shot_dist < 6.0f && sense.shot_closing)
			near_miss += dt;
	}

	const float alpha = genome_.reaction;
	direction.x = direction.x * (1.0f - alpha) + desired.x * alpha;
	direction.y = direction.y * (1.0f - alpha) + desired.y * alpha;

	const float mag = static_cast<float>(
		std::sqrt(direction.x * direction.x + direction.y * direction.y));
	if (mag > 1.75f) {
		direction.x *= 1.75f / mag;
		direction.y *= 1.75f / mag;
	}

	center += direction * dt * genome_.speed;

	if (center.x < -9.0f) {
		center.x = -9.0f;
		direction.x = std::abs(direction.x);
	}
	if (center.x > 9.0f) {
		center.x = 9.0f;
		direction.x = -std::abs(direction.x);
	}
	if (center.y < -9.0f) {
		center.y = -9.0f;
		direction.y = std::abs(direction.y);
	}
	if (center.y > 9.0f) {
		center.y = 9.0f;
		direction.y = -std::abs(direction.y);
	}

	generate();
}

}
