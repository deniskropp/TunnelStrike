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

	const int ticks = 2000;
	for (int i = 0; i < ticks; ++i) {
		Camera3d::instance().translate(Vector3d(0, 0, 0.04f));
		world.Tick(clock.step());
		clock.noteSteps(1);
	}

	world.store().snapshotWorld(world, world.evo(), clock.totalSteps());
	world.store().appendGeneration(
		world.evo().generationIndex(),
		world.evo().lastBestFitness(),
		world.evo().pool());

	std::cout << "headless ticks=" << clock.totalSteps()
		<< " kills=" << world.get_kills()
		<< " generation=" << world.generation()
		<< " pending=" << world.evo().scoredPending()
		<< " best=" << world.evo().lastBestFitness()
		<< std::endl;

	return 0;
}
