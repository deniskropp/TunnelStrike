#pragma once

#include <SFML/Graphics.hpp>


namespace TunnelStrike {


	class World;

	class Entity : public sf::Drawable
	{
		friend class World;

	protected:
		World& world;
		sf::Time age;

	public:
		Entity(World& world) : world(world) {}

		virtual void Tick(sf::Time delta) = 0;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
	};


}