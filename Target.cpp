#include "Target.hpp"


namespace TunnelStrike {


	Target::Target(sf::Vector3f center, float size, sf::Color color)
		:
		center(center.x, center.y, center.z)
	{
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y - size / 2.0f, center.z + size / 2.0f), color, color));
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y - size / 2.0f, center.z + size / 2.0f), color, color));
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y + size / 2.0f, center.z + size / 2.0f), color, color));
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y + size / 2.0f, center.z + size / 2.0f), color, color));

		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y - size / 2.0f, center.z - size / 2.0f), color, color));
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y - size / 2.0f, center.z - size / 2.0f), color, color));
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x - size / 2.0f, center.y + size / 2.0f, center.z - size / 2.0f), color, color));
		segments.push_back(Segment3d(Vector3d(center.x, center.y, center.z), Vector3d(center.x + size / 2.0f, center.y + size / 2.0f, center.z - size / 2.0f), color, color));
	}


}