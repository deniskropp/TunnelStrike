#include "Targets.hpp"

#include "geometry/camera3d.hpp"


namespace TunnelStrike {


	Targets::Targets(World& world)
		:
		Entity(world),
		lz(0.0)
	{
	}

	void Targets::Tick()
	{
		/* Build up list of still existing targets */
		std::set<std::shared_ptr<Target>> targets_new;

		for (auto t : targets) {
			/* Remove targets that got behind the camera */
			if (t->GetCenter().get_z() <= Camera3d::instance().center().get_z())
				continue;

			/* keep target */
			targets_new.insert(t);
		}

		targets = targets_new;

		/* Insert further targets */
		float cz = (float)Camera3d::instance().center().get_z();

		if (cz - lz > 100.0f) {
			lz = cz;

			sf::Vector3f pos(::rand() % 11 - 5.0f, ::rand() % 11 - 5.0f, (float)Camera3d::instance().center().get_z() + 500.0f + (float)(::rand() % 200));

			int v = ::rand() % 256;

			sf::Color color(v, 255 - v, ::rand() % 256);

			auto target = std::make_shared<Target>(pos, 3.0f, color);

			targets.insert(target);
		}
	}

	void Targets::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// draw targets
		for (auto t : targets)
			target.draw(*t);
	}


}