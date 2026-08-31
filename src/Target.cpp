#include "Target.hpp"

#include <cmath>


namespace TunnelStrike {

Target::Target(sf::Vector3f spawn, const Genome &genome)
	:
	center(spawn.x, spawn.y, spawn.z),
	size(genome.size),
	color(genome.color()),
	genome_(genome)
{
	direction = Vector3d(genome_.vx, genome_.vy, 0.0);
	generate();
}

void Target::generate()
{
	segments.clear();

	const int n = std::max(8, genome_.morph_segments);
	const float spread = genome_.morph_spread;

	for (int i = 0; i < n; i++) {
		const float ox = static_cast<float>(fmod((::rand() % 1001 - 500), spread));
		const float oy = static_cast<float>(fmod((::rand() % 1001 - 500), spread));
		segments.push_back(Segment3d(
			Vector3d(center.x, center.y, center.z),
			Vector3d(center.x - ox, center.y - oy, center.z),
			color, color));
	}
}

void Target::Act(sf::Time delta)
{
	const float dt = delta.asSeconds();
	lived += dt;

	const int period = std::max(1, static_cast<int>(1.0f / std::max(genome_.jitter, 0.001f)));
	if ((::rand() % period) == 0) {
		direction.x = genome_.vx + static_cast<float>(::rand() % 1001) / 1000.0f - 0.5f;
		direction.y = genome_.vy + static_cast<float>(::rand() % 1001) / 1000.0f - 0.5f;
	}

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
