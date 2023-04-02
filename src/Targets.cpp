#include "Targets.hpp"

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
		/* Build up list of still existing targets */
		std::list<std::shared_ptr<Target>> targets_new;

		for (auto t : targets) {
			/* Remove targets that got behind the camera */
			if (t->GetCenter().get_z() <= Camera3d::instance().center().get_z())
				continue;

			/* keep target */
			targets_new.push_back(t);


			t->Act(delta);
		}

		targets = targets_new;

		/* Insert further targets */
		float cz = (float)Camera3d::instance().center().get_z();

		if (cz - lz > 30.0f) {
			lz = cz;

			sf::Vector3f pos(::rand() % 11 - 5.0f, ::rand() % 11 - 5.0f, (float)Camera3d::instance().center().get_z() + 500.0f + (float)(::rand() % 200));

			int v = ::rand() % 256;

			sf::Color color(v, 255, 255 - v);
			//sf::Color color(255, 255, 255);

			auto target = std::make_shared<Target>(pos, 3.0f, color);

			targets.push_back(target);
		}
	}

	void Targets::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		// draw targets
		for (auto t : targets)
			target.draw(*t);
	}


}