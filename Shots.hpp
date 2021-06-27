#pragma once

#include <memory>
#include <vector>

#include "Entity.hpp"
#include "Shot.hpp"


namespace TunnelStrike {


	class Shots : public Entity
	{
		friend class Main;

	private:
		std::vector<std::shared_ptr<Shot>> shots;

	public:
		Shots(World& world)
			:
			Entity(world)
		{
		}

		virtual void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};


}
