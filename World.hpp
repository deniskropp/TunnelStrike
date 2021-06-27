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
		friend class Main;
		friend class Shots;

	private:
		std::vector<Entity*> entities;

	protected:
		std::unique_ptr<Crosshair> crosshair;
		std::unique_ptr<Shots> shots;
		std::unique_ptr<Targets> targets;
		std::unique_ptr<Walls> walls;

		unsigned int kills;

	public:
		World();

		void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		void killed()
		{
			kills++;
		}

		unsigned int get_kills() const { return kills; }
	};


}