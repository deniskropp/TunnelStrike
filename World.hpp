#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Crosshair.hpp"
#include "Shots.hpp"
#include "Targets.hpp"
#include "Walls.hpp"


namespace TunnelStrike {


	class World : public sf::Drawable
	{
	private:
		std::vector<Entity*> entities;

	public:
		std::unique_ptr<Crosshair> crosshair;
		std::unique_ptr<Shots> shots;
		std::unique_ptr<Targets> targets;
		std::unique_ptr<Walls> walls;

		World();

		void Tick();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};


}