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

		const Vector3d player = Camera3d::instance().center();
		const std::vector<World::LiveShot> shots = world.liveShots();

		for (auto t : targets) {
			if (t->GetCenter().get_z() <= player.get_z()) {
				world.recordFitness(t->genome(), t->fitnessIfSurvived());
				continue;
			}

			Sense sense;
			sense.player = player;
			for (const auto &shot : shots) {
				const float d = static_cast<float>(t->GetCenter().distance_to(shot.pos));
				if (d < sense.nearest_shot_dist) {
					sense.nearest_shot_dist = d;
					sense.nearest_shot = shot.pos;
					sense.nearest_shot_dir = shot.dir;
					sense.has_shot = true;
					const Vector3d to_target = t->GetCenter() - shot.pos;
					sense.shot_closing = (to_target * shot.dir) > 0.0;
				}
			}

			targets_new.push_back(t);
			t->Act(delta, sense);
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
