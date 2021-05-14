#pragma once

#include <memory>
#include <set>

#include "Entity.hpp"
#include "Target.hpp"


namespace TunnelStrike {


	class Targets : public Entity
	{
		friend class Shots;

	private:
		std::set<std::shared_ptr<Target>> targets;
		double lz;

	public:
		Targets(World& world);

		virtual void Tick();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};


}