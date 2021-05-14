#pragma once

#include <SFML/Graphics.hpp>


namespace TunnelStrike {


	class World;

	class Entity : public sf::Drawable
	{
	protected:
		World& world;

	public:
		Entity(World& world) : world(world) {}

		virtual void Tick() = 0;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	};


}