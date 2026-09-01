#include "Target.hpp"

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
		direction.x = genome_.vx + unit(rng_) - 0.5f;
		direction.y = genome_.vy + unit(rng_) - 0.5f;
	}

	Vector3d steer(direction.x, direction.y, 0.0);

	Vector3d to_player(sense.player.x - center.x, sense.player.y - center.y, 0.0);
	if (to_player.norm() > 1e-4) {
		to_player.normalize();
		steer.x += to_player.x * genome_.seek;
		steer.y += to_player.y * genome_.seek;
	}

	if (sense.has_shot) {
		Vector3d away(
			center.x - sense.nearest_shot.x,
			center.y - sense.nearest_shot.y,
			0.0);
		if (away.norm() > 1e-4) {
			away.normalize();
			steer.x += away.x * genome_.dodge;
			steer.y += away.y * genome_.dodge;
		}
	}

	const float alpha = genome_.reaction;
	direction.x = direction.x * (1.0f - alpha) + steer.x * alpha;
	direction.y = direction.y * (1.0f - alpha) + steer.y * alpha;

	center += direction * dt * genome_.speed;

	if (center.x < -9.0f)
		center.x = -9.0f;

	if (center.x > 9.0f)
		center.x = 9.0f;

	if (center.y < -9.0f)
		center.y = -9.0f;

	if (center.y > 9.0f)
		center.y = 9.0f;

	generate();
}

}
