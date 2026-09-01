#include "Sfx.hpp"
#include "World.hpp"


namespace TunnelStrike {


	World::World(std::string evo_dir)
		:
		kills(0),
		archive(std::move(evo_dir))
	{
		crosshair = std::make_unique<Crosshair>(*this);
		shots = std::make_unique<Shots>(*this);
		targets = std::make_unique<Targets>(*this);
		walls = std::make_unique<Walls>(*this);

		entities.push_back(crosshair.get());
		entities.push_back(shots.get());
		entities.push_back(walls.get());
		entities.push_back(targets.get());

		std::vector<Genome> loaded;
		unsigned generation = 0;
		if (archive.loadLatestPool(loaded, &generation))
			population.seedFrom(loaded, generation);
	}

	void World::fire(const Vector3d &pos, const Vector3d &dir)
	{
		pending_shots.emplace_back(pos, dir);
	}

	void World::recordFitness(const Genome &genome, float base)
	{
		population.record(genome, base + archive.assessmentBonus(genome));
	}

	std::vector<Vector3d> World::liveTargetCenters() const
	{
		std::vector<Vector3d> out;
		if (!targets)
			return out;

		out.reserve(targets->targets.size());
		for (const auto &t : targets->targets)
			out.push_back(t->GetCenter());
		return out;
	}

	std::vector<Vector3d> World::liveShotPositions() const
	{
		std::vector<Vector3d> out;
		if (!shots)
			return out;

		out.reserve(shots->shots.size());
		for (const auto &s : shots->shots)
			out.push_back(s->GetCenter());
		return out;
	}

	void World::Tick(sf::Time delta)
	{
		++ticks;

		for (const auto &queued : pending_shots)
			shots->spawn(queued.first, queued.second);
		pending_shots.clear();

		for (auto entity : entities) {
			entity->age += delta;

			entity->Tick(delta);
		}

		if (population.maybeEvolve()) {
			archive.appendGeneration(
				population.generationIndex(),
				population.lastBestFitness(),
				population.lastMeanFitness(),
				population.lastDiversity(),
				population.pool());
		}

		if (ticks % 120ull == 0)
			archive.snapshotWorld(*this, population, ticks);
	}

	void World::draw(sf::RenderTarget& target, DrawStates states) const
	{
		for (auto entity : entities)
			entity->draw(target, states);
	}

	void World::killed()
	{
		kills++;

		Sfx::instance().PlayDie();
	}


}
