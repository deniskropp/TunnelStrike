#pragma once

#include "Entity.hpp"


namespace TunnelStrike {


	class Crosshair : public Entity
	{
	private:
		sf::VertexArray v;

	public:
		Crosshair(World& world);

		virtual void Tick();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};

}