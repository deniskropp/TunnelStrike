#include <iostream>

#include "geometry/camera3d.hpp"
#include "sim/SimClock.hpp"
#include "World.hpp"

int main()
{
	using namespace TunnelStrike;

	Camera3d::instance().translate(Vector3d(0, 0, 100.0f));

	World world;
	SimClock clock;

	const int ticks = 18000;
	for (int i = 0; i < ticks; ++i) {
		Camera3d::instance().translate(Vector3d(0, 0, 0.04f));

		if (i % 20 == 0) {
			const Vector3d origin(0, 0, Camera3d::instance().center().get_z());
			for (const Vector3d &center : world.liveTargetCenters()) {
				Vector3d dir = center - origin;
				if (dir.norm() < 1e-6)
					continue;
				dir.normalize();
				dir *= 100.0;
				world.fire(origin, dir);
			}
		}

		world.Tick(clock.step());
		clock.noteSteps(1);
	}

	world.store().snapshotWorld(world, world.evo(), clock.totalSteps());

	std::cout << "headless ticks=" << clock.totalSteps()
		<< " kills=" << world.get_kills()
		<< " generation=" << world.generation()
		<< " pending=" << world.evo().scoredPending()
		<< " best=" << world.evo().lastBestFitness()
		<< std::endl;

	if (world.generation() == 0) {
		std::cerr << "headless verify failed: generation stayed 0\n";
		return 1;
	}

	return 0;
}
