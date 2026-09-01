#pragma once

#include <memory>
#include <vector>

#include "Entity.hpp"
#include "Shot.hpp"


namespace TunnelStrike {


	class Shots : public Entity
	{
		friend class Main;
		friend class World;

	private:
		std::vector<std::shared_ptr<Shot>> shots;

	public:
		Shots(World& world)
			:
			Entity(world)
		{
		}

		void spawn(const Vector3d &pos, const Vector3d &dir);

		virtual void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, DrawStates states) const;
	};


}
