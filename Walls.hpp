#pragma once

#include <vector>

#include "Entity.hpp"
#include "Quad.hpp"


namespace TunnelStrike {


	class Walls : public Entity
	{
	private:
		float maxz;
		std::vector<Quad> q;

	public:
		Walls(World& world);

		virtual void Tick();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};


}