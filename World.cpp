#include "World.hpp"


namespace TunnelStrike {


	World::World()
	{
		crosshair = std::make_unique<Crosshair>(*this);
		shots = std::make_unique<Shots>(*this);
		targets = std::make_unique<Targets>(*this);
		walls = std::make_unique<Walls>(*this);

		entities.push_back(walls.get());
		entities.push_back(targets.get());
		entities.push_back(shots.get());
		entities.push_back(crosshair.get());
	}

	void World::Tick()
	{
		for (auto entity : entities)
			entity->Tick();
	}

	void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for (auto entity : entities)
			entity->draw(target, states);
	}


}
