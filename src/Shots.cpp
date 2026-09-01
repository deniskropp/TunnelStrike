#include <memory>
#include <vector>

#include "Entity.hpp"
#include "Sfx.hpp"
#include "Shots.hpp"
#include "Targets.hpp"
#include "World.hpp"


namespace TunnelStrike {


	void Shots::spawn(const Vector3d &pos, const Vector3d &dir)
	{
		shots.push_back(std::make_shared<Shot>(pos, dir));
	}

	void Shots::Tick(sf::Time delta)
	{
		std::vector<std::shared_ptr<Shot>> shots_new;

		for (auto s : shots) {
			s->Tick(delta);

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

	void Shots::draw(sf::RenderTarget& target, DrawStates states) const
	{
		for (auto s : shots)
			target.draw(*s);
	}


}
