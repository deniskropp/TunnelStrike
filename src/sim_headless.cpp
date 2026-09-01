#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "evo/Pilot.hpp"
#include "evo/Population.hpp"
#include "geometry/camera3d.hpp"
#include "persist/Archive.hpp"
#include "sim/SimClock.hpp"
#include "Target.hpp"
#include "World.hpp"

namespace {

	int fail(const char *msg)
	{
		std::cerr << "headless verify failed: " << msg << '\n';
		return 1;
	}

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
		if (a.bodySegmentCount() != b.bodySegmentCount())
			return fail("structure is not deterministic");
		if (a.bodySegmentCount() != g.bodySegmentCount())
			return fail("target segments != genome formula");
		if (a.bodySegmentCount() < 4)
			return fail("structured body too small");

		Genome g2 = g;
		g2.limbs = 7;
		const Target c(pos, g2);
		if (c.bodySegmentCount() == a.bodySegmentCount())
			return fail("limb gene does not change topology");

		g.seek = -0.4f;
		g.dodge = 1.1f;
		g.reaction = 0.7f;
		g.strafe = 0.9f;
		g.range = 6.5f;
		g.lead = 1.1f;
		const Genome round = Genome::fromLine(g.toLine());
		if (round.limbs != g.limbs || std::abs(round.fork - g.fork) > 1e-4f)
			return fail("genome roundtrip lost structure genes");
		if (std::abs(round.seek - g.seek) > 1e-4f ||
			std::abs(round.dodge - g.dodge) > 1e-4f ||
			std::abs(round.reaction - g.reaction) > 1e-4f)
			return fail("genome roundtrip lost motion genes");
		if (std::abs(round.strafe - g.strafe) > 1e-4f ||
			std::abs(round.range - g.range) > 1e-4f ||
			std::abs(round.lead - g.lead) > 1e-4f)
			return fail("genome roundtrip lost steer genes");

		const Genome old = Genome::fromLine(
			"-0.03 -0.12 0.05 109 5.4 3.6 51 0.38 0.75 0.22");
		if (old.morph_segments != 51 || old.limbs != 5)
			return fail("old pool line parse");
		if (old.dodge < 0.0f || old.reaction < 0.05f)
			return fail("old pool line lost motion defaults");
		if (old.strafe < 0.0f || old.range < 1.0f || old.lead < 0.0f)
			return fail("old pool line lost steer defaults");

		const Genome mid = Genome::fromLine(
			"0.1 -0.2 0.04 90 3 3 40 0.4 0.8 0.3 5 1.0 0.2 0.1 -0.2 0.5 0.4");
		if (std::abs(mid.seek + 0.2f) > 1e-3f || std::abs(mid.range - 4.0f) > 1e-3f)
			return fail("17-field line lost range default");

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
		if (std::abs(exact - 3.75f) > 0.01f)
			return fail("assess exact+wildcard");
		if (std::abs(wild - 2.25f) > 0.01f)
			return fail("assess wildcard");

		return 0;
	}

	int verify_ga()
	{
		using namespace TunnelStrike;

		std::mt19937 rng(42);
		Genome a = Genome::random(rng);
		Genome hot = a.mutated(rng, 1.0f);
		hot.clamp();
		if (hot.speed < 20.0f || hot.speed > 200.0f)
			return fail("mutated speed out of bounds");
		if (hot.seek < -1.0f || hot.seek > 1.0f)
			return fail("mutated seek out of bounds");
		if (hot.dodge < 0.0f || hot.dodge > 1.5f)
			return fail("mutated dodge out of bounds");
		if (hot.reaction < 0.05f || hot.reaction > 1.0f)
			return fail("mutated reaction out of bounds");
		if (hot.strafe < 0.0f || hot.strafe > 1.5f)
			return fail("mutated strafe out of bounds");
		if (hot.range < 1.0f || hot.range > 9.0f)
			return fail("mutated range out of bounds");
		if (hot.lead < 0.0f || hot.lead > 1.5f)
			return fail("mutated lead out of bounds");

		Genome mixed = Genome::crossover(a, hot, rng);
		if (mixed.limbs < 3 || mixed.limbs > 8)
			return fail("crossover limbs out of bounds");

		Population p;
		Genome champ = Genome::random(rng);
		champ.speed = 173.0f;
		champ.seek = -0.55f;
		p.record(champ, 100.0f);
		for (int i = 0; i < 15; ++i) {
			Genome g = Genome::random(rng);
			p.record(g, static_cast<float>(i + 1));
		}
		if (!p.maybeEvolve())
			return fail("population did not evolve at batch size");
		if (p.generationIndex() != 1)
			return fail("generation did not increment");
		if (p.pool().size() != Population::POOL_SIZE)
			return fail("evolved pool size");
		if (p.lastBestFitness() < 100.0f - 1e-4f)
			return fail("elite best fitness dropped");
		if (std::abs(p.pool().front().speed - champ.speed) > 1e-3f ||
			std::abs(p.pool().front().seek - champ.seek) > 1e-3f)
			return fail("best genome was not cloned as elite");
		if (p.lastMeanFitness() <= 0.0f)
			return fail("mean fitness not recorded");
		if (p.lastDiversity() < 0.0f)
			return fail("diversity not recorded");

		const std::string dir = "/tmp/tunnelstrike-ga-verify";
		{
			Archive arch(dir);
			arch.appendGeneration(
				p.generationIndex(),
				p.lastBestFitness(),
				p.lastMeanFitness(),
				p.lastDiversity(),
				p.pool());
		}

		std::vector<Genome> loaded;
		unsigned gen = 0;
		const Archive loaded_arch(dir);
		if (!loaded_arch.loadLatestPool(loaded, &gen))
			return fail("could not reload evolved pool");
		if (gen != 1)
			return fail("generation was not persisted");
		if (loaded.size() != Population::POOL_SIZE)
			return fail("persisted pool size");

		Population restored;
		restored.seedFrom(loaded, gen);
		if (restored.generationIndex() != 1)
			return fail("seedFrom dropped generation");

		for (int i = 0; i < 16; ++i)
			restored.record(Genome::random(rng), 1.0f + static_cast<float>(i));
		if (!restored.maybeEvolve() || restored.generationIndex() != 2)
			return fail("second generation did not advance");

		return 0;
	}

	int verify_motion_sense()
	{
		using namespace TunnelStrike;

		Genome g;
		g.vx = 0.0f;
		g.vy = 0.0f;
		g.jitter = 0.001f;
		g.speed = 80.0f;
		g.seek = 0.0f;
		g.dodge = 1.2f;
		g.reaction = 1.0f;
		g.strafe = 0.0f;
		g.range = 4.0f;
		g.lead = 0.0f;
		g.size = 3.0f;
		g.limb_len = 1.0f;
		g.limbs = 4;

		const sf::Vector3f pos(0.f, 0.f, 100.f);
		Target calm(pos, g);
		Target hunted(pos, g);

		Sense none;
		none.player = Vector3d(0, 0, 0);

		Sense threat;
		threat.player = Vector3d(0, 0, 0);
		threat.has_shot = true;
		threat.shot_closing = true;
		threat.nearest_shot = Vector3d(2.0, 0.0, 100.0);
		threat.nearest_shot_dir = Vector3d(-1.0, 0.0, 0.0);
		threat.nearest_shot_dist = 2.0f;

		const sf::Time dt = sf::seconds(0.03f);
		for (int i = 0; i < 12; ++i) {
			calm.Act(dt, none);
			hunted.Act(dt, threat);
		}

		if (hunted.GetCenter().x >= calm.GetCenter().x - 0.05)
			return fail("dodge gene did not move away from shot");
		if (hunted.fitnessIfKilled() <= calm.fitnessIfKilled())
			return fail("near-miss did not raise killed fitness");

		g.dodge = 0.0f;
		g.seek = 1.0f;
		g.range = 4.0f;
		Target seeker(pos, g);
		Sense lure;
		lure.player = Vector3d(4.0, 0.0, 100.0);
		for (int i = 0; i < 12; ++i)
			seeker.Act(dt, lure);

		if (seeker.GetCenter().x <= 0.05)
			return fail("seek gene did not move toward player");

		g.seek = 0.0f;
		g.range = 8.0f;
		g.dodge = 0.0f;
		g.strafe = 0.0f;
		const sf::Vector3f near_pos(1.f, 0.f, 100.f);
		Target spacer(near_pos, g);
		Sense close_player;
		close_player.player = Vector3d(0, 0, 100.0);
		for (int i = 0; i < 12; ++i)
			spacer.Act(dt, close_player);
		if (spacer.GetCenter().x <= 1.05)
			return fail("range gene did not open distance from player");

		g.range = 4.0f;
		g.dodge = 0.0f;
		g.strafe = 1.3f;
		g.twist = 0.2f;
		Target slider(pos, g);
		Sense side;
		side.player = Vector3d(0, 0, 0);
		side.has_shot = true;
		side.shot_closing = true;
		side.nearest_shot = Vector3d(-2.0, 0.0, 100.0);
		side.nearest_shot_dir = Vector3d(1.0, 0.0, 0.0);
		side.nearest_shot_dist = 2.0f;
		for (int i = 0; i < 12; ++i)
			slider.Act(dt, side);
		if (std::abs(slider.GetCenter().y) < 0.08)
			return fail("strafe gene did not move perpendicular to shot");

		return 0;
	}

	int verify_pilot()
	{
		using namespace TunnelStrike;

		std::mt19937 rng(7);
		Pilot a = Pilot::random(rng);
		a.lead = 1.25f;
		a.trigger = 0.8f;
		a.stick = 0.82f;
		a.max_dist = 520.0f;
		const Pilot round = Pilot::fromLine(a.toLine());
		if (std::abs(round.lead - a.lead) > 1e-4f || std::abs(round.trigger - a.trigger) > 1e-4f)
			return fail("pilot genome roundtrip");
		if (std::abs(round.stick - a.stick) > 1e-4f || std::abs(round.max_dist - a.max_dist) > 1e-2f)
			return fail("pilot genome roundtrip lost stick/range");

		const Pilot oldp = Pilot::fromLine("1.0 0.5 0.01 0.26 560 0.65 0.2 0.08 0.5 0 0");
		if (oldp.stick < 0.2f || oldp.max_dist < 120.0f)
			return fail("old pilot line lost stick default");

		Pilot hot = a.mutated(rng, 1.0f);
		if (hot.fire_gap < 0.15f || hot.fire_gap > 0.7f)
			return fail("mutated fire_gap out of bounds");
		if (hot.max_dist < 120.0f || hot.max_dist > 780.0f)
			return fail("mutated max_dist out of bounds");
		if (hot.stick < 0.2f || hot.stick > 0.95f)
			return fail("mutated stick out of bounds");

		PilotPop p;
		Pilot champ = Pilot::random(rng);
		champ.lead = 1.7f;
		p.record(champ, 50.0f);
		for (int i = 0; i < 7; ++i)
			p.record(Pilot::random(rng), static_cast<float>(i + 1));
		if (!p.maybeEvolve())
			return fail("pilot population did not evolve");
		if (p.generationIndex() != 1)
			return fail("pilot generation did not increment");
		if (std::abs(p.pool().front().lead - champ.lead) > 1e-3f)
			return fail("pilot elite was not cloned");

		const std::string dir = "/tmp/tunnelstrike-pilot-verify";
		{
			Archive arch(dir);
			arch.appendPilot(
				p.generationIndex(),
				p.lastBestFitness(),
				p.lastMeanFitness(),
				p.lastDiversity(),
				p.pool());
		}
		std::vector<Pilot> loaded;
		unsigned gen = 0;
		const Archive loaded_arch(dir);
		if (!loaded_arch.loadLatestPilot(loaded, &gen) || gen != 1 || loaded.size() != PilotPop::POOL_SIZE)
			return fail("pilot pool persist");

		return 0;
	}

}

int main()
{
	using namespace TunnelStrike;

	if (const int rc = verify_structure())
		return rc;
	if (const int rc = verify_ga())
		return rc;
	if (const int rc = verify_motion_sense())
		return rc;
	if (const int rc = verify_pilot())
		return rc;

	Camera3d::instance().translate(Vector3d(0, 0, 100.0f));

	World world("/tmp/tunnelstrike-sim-evo");
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
		<< " mean=" << world.evo().lastMeanFitness()
		<< " diversity=" << world.evo().lastDiversity()
		<< std::endl;

	if (world.generation() == 0)
		return fail("generation stayed 0");

	return 0;
}
