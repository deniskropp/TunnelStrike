#include "Target.hpp"


namespace TunnelStrike {


Target::Target(sf::Vector3f center, float size, sf::Color color)
	:
	center(center.x, center.y, center.z),
	size(size),
	color(color)
{
	generate();
}

void Target::generate()
{
	segments.clear();

	for (int i=0; i<40; i++)
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z),
			                         Vector3d(center.x - fmod((::rand()%1001 - 500), size), center.y - fmod((::rand() % 1001 - 500), size), center.z),
			                         color, color));

//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y - size / 2.0f, center.z + size / 2.0f), color, color));
//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y - size / 2.0f, center.z + size / 2.0f), color, color));
//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y + size / 2.0f, center.z + size / 2.0f), color, color));
//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y + size / 2.0f, center.z + size / 2.0f), color, color));

//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y - size / 2.0f, center.z - size / 2.0f), color, color));
//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y - size / 2.0f, center.z - size / 2.0f), color, color));
//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y + size / 2.0f, center.z - size / 2.0f), color, color));
//	segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y + size / 2.0f, center.z - size / 2.0f), color, color));
}

void Target::Act(sf::Time delta)
{
	if (::rand() % 100 == 0) {
		direction.x = ::rand() % 1001 / 1000.0f - 0.5f;
		direction.y = ::rand() % 1001 / 1000.0f - 0.5f;
	}

	center += direction * delta.asSeconds() * 100.0f;

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