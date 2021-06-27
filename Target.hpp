#pragma once

#include "geometry/segmented.hpp"


namespace TunnelStrike {


	class Target : public Segmented
	{
	private:
		Vector3d center;
		float size;
		sf::Color color;
		Vector3d direction;

		void generate();

	public:
		Target(sf::Vector3f center, float size, sf::Color color);

		Vector3d GetCenter() const { return center; }

		void Act(sf::Time delta);
	};


}