#include "Targets.hpp"

#include "World.hpp"
#include "geometry/camera3d.hpp"


namespace TunnelStrike {


	Targets::Targets(World& world)
		:
		Entity(world),
		lz(0.0)
	{
	}

	void Targets::Tick(sf::Time delta)
	{
		std::list<std::shared_ptr<Target>> targets_new;

		for (auto t : targets) {
			if (t->GetCenter().get_z() <= Camera3d::instance().center().get_z()) {
				world.evo().record(t->genome(), t->fitnessIfSurvived());
				continue;
			}

			targets_new.push_back(t);
			t->Act(delta);
		}

		targets = targets_new;

		float cz = (float)Camera3d::instance().center().get_z();

		if (cz - lz > 30.0f) {
			lz = cz;

			sf::Vector3f pos(::rand() % 11 - 5.0f, ::rand() % 11 - 5.0f, (float)Camera3d::instance().center().get_z() + 500.0f + (float)(::rand() % 200));

			const Genome g = world.evo().nextSpawn();
			auto target = std::make_shared<Target>(pos, g);

			targets.push_back(target);
		}
	}

	void Targets::draw(sf::RenderTarget& target, DrawStates states) const
	{
		for (auto t : targets)
			target.draw(*t);
	}


}
