#pragma once

#include "Entity.hpp"


namespace TunnelStrike {


	class Crosshair : public Entity
	{
	private:
		sf::VertexArray v;

	public:
		Crosshair(World& world);

		virtual void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;
	};

}