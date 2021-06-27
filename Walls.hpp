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
		sf::Shader shader;

	public:
		Walls(World& world);

		virtual void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};


}