#include "Sfx.hpp"
#include "World.hpp"


namespace TunnelStrike {


	World::World()
		:
		kills(0)
	{
		crosshair = std::make_unique<Crosshair>(*this);
		shots = std::make_unique<Shots>(*this);
		targets = std::make_unique<Targets>(*this);
		walls = std::make_unique<Walls>(*this);

		entities.push_back(crosshair.get());
		entities.push_back(shots.get());
		entities.push_back(walls.get());
		entities.push_back(targets.get());
	}

	void World::Tick(sf::Time delta)
	{
		for (auto entity : entities) {
			entity->age += delta;

			entity->Tick(delta);
		}
	}

	void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for (auto entity : entities)
			entity->draw(target, states);
	}

	void World::killed()
	{
		kills++;

		Sfx::instance().PlayDie();
	}

}
