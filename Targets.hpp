#pragma once

#include <list>
#include <memory>

#include "Entity.hpp"
#include "Target.hpp"


namespace TunnelStrike {


	class Targets : public Entity
	{
		friend class Main;
		friend class Shots;

	private:
		std::list<std::shared_ptr<Target>> targets;
		double lz;

	public:
		Targets(World& world);

		virtual void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};


}