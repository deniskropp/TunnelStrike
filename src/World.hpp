#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Crosshair.hpp"
#include "Shots.hpp"
#include "Targets.hpp"
#include "Walls.hpp"
#include "evo/Population.hpp"
#include "persist/Archive.hpp"


namespace TunnelStrike {



	class World : public sf::Drawable
	{
		friend class Main;
		friend class Shots;
		friend class Archive;
		friend class Targets;

	private:
		std::vector<Entity*> entities;

	protected:
		std::unique_ptr<Crosshair> crosshair;
		std::unique_ptr<Shots> shots;
		std::unique_ptr<Targets> targets;
		std::unique_ptr<Walls> walls;

		unsigned int kills;
		unsigned long long ticks = 0;

		Population population;
		Archive archive;

	public:
		World();

		void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, DrawStates states) const;

		void killed();

		unsigned int get_kills() const { return kills; }

		Population &evo() { return population; }
		const Population &evo() const { return population; }

		Archive &store() { return archive; }

		unsigned long long tickIndex() const { return ticks; }

		unsigned generation() const { return population.generationIndex(); }
	};


}
