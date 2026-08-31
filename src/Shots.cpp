#include <memory>
#include <vector>

#include "Entity.hpp"
#include "Sfx.hpp"
#include "Shots.hpp"
#include "Targets.hpp"
#include "World.hpp"


namespace TunnelStrike {


	void Shots::Tick(sf::Time delta)
	{
		std::vector<std::shared_ptr<Shot>> shots_new;

		for (auto s : shots) {
			std::shared_ptr<Target> target = s->hitTarget(world.targets->targets);

			if (target) {
				Sfx::instance().PlayHit();

				world.evo().record(target->genome(), target->fitnessIfKilled());

				for (auto it = world.targets->targets.begin(); it != world.targets->targets.end(); it++) {
					if (*it == target) {
						world.targets->targets.erase(it);
						break;
					}
				}

				world.killed();
				continue;
			}

			if (s->outsideWalls()) {
				Sfx::instance().PlayHit();
				continue;
			}

			shots_new.push_back(s);
		}

		shots = shots_new;
	}

	void Shots::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		for (auto s : shots)
			target.draw(*s);
	}


}
