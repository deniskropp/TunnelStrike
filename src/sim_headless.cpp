#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "geometry/camera3d.hpp"
#include "persist/Archive.hpp"
#include "sim/SimClock.hpp"
#include "Target.hpp"
#include "World.hpp"

namespace {

	int verify_structure()
	{
		using namespace TunnelStrike;

		Genome g;
		g.limbs = 4;
		g.limb_len = 1.2f;
		g.fork = 0.8f;
		g.twist = 0.5f;
		g.morph_segments = 40;
		g.size = 3.0f;
		g.morph_spread = 2.0f;

		const sf::Vector3f pos(0.f, 0.f, 100.f);
		const Target a(pos, g);
		const Target b(pos, g);
		if (a.bodySegmentCount() != b.bodySegmentCount()) {
			std::cerr << "headless verify failed: structure is not deterministic\n";
			return 1;
		}
		if (a.bodySegmentCount() != g.bodySegmentCount()) {
			std::cerr << "headless verify failed: target segments != genome formula\n";
			return 1;
		}
		if (a.bodySegmentCount() < 4) {
			std::cerr << "headless verify failed: structured body too small\n";
			return 1;
		}

		Genome g2 = g;
		g2.limbs = 7;
		const Target c(pos, g2);
		if (c.bodySegmentCount() == a.bodySegmentCount()) {
			std::cerr << "headless verify failed: limb gene does not change topology\n";
			return 1;
		}

		const Genome round = Genome::fromLine(g.toLine());
		if (round.limbs != g.limbs || std::abs(round.fork - g.fork) > 1e-4f) {
			std::cerr << "headless verify failed: genome roundtrip lost structure genes\n";
			return 1;
		}

		const Genome old = Genome::fromLine(
			"-0.03 -0.12 0.05 109 5.4 3.6 51 0.38 0.75 0.22");
		if (old.morph_segments != 51 || old.limbs != 5) {
			std::cerr << "headless verify failed: old pool line parse\n";
			return 1;
		}

		const std::string dir = "/tmp/tunnelstrike-assess-verify";
		{
			Archive create_dir(dir);
			std::ofstream file(dir + "/assess.txt");
			file << "* 2.25\n";
			file << "1.5 " << g.toLine() << '\n';
		}
		const Archive arch(dir);
		const float exact = arch.assessmentBonus(g);
		std::mt19937 rng(1);
		Genome other = Genome::random(rng);
		other.limbs = 3;
		const float wild = arch.assessmentBonus(other);
		if (std::abs(exact - 3.75f) > 0.01f) {
			std::cerr << "headless verify failed: assess exact+wildcard " << exact << "\n";
			return 1;
		}
		if (std::abs(wild - 2.25f) > 0.01f) {
			std::cerr << "headless verify failed: assess wildcard " << wild << "\n";
			return 1;
		}

		return 0;
	}

}

int main()
{
	using namespace TunnelStrike;

	if (const int rc = verify_structure())
		return rc;

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
