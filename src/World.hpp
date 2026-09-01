#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Crosshair.hpp"
#include "Shots.hpp"
#include "Targets.hpp"
#include "Walls.hpp"
#include "evo/Population.hpp"
#include "persist/Archive.hpp"


namespace TunnelStrike {



	class World : public sf::Drawable
	{
		friend class Main;
		friend class Shots;
		friend class Archive;
		friend class Targets;

	private:
		std::vector<Entity*> entities;

	protected:
		std::unique_ptr<Crosshair> crosshair;
		std::unique_ptr<Shots> shots;
		std::unique_ptr<Targets> targets;
		std::unique_ptr<Walls> walls;

		unsigned int kills;
		unsigned int misses = 0;
		unsigned long long ticks = 0;

		Population population;
		Archive archive;

		std::vector<std::pair<Vector3d, Vector3d>> pending_shots;

	public:
		explicit World(std::string evo_dir = "evo");

		void Tick(sf::Time delta);

		void fire(const Vector3d &pos, const Vector3d &dir);
		struct LiveShot
		{
			Vector3d pos;
			Vector3d dir;
		};

		struct LiveTarget
		{
			Vector3d pos;
			Vector3d vel;
			float size = 3.0f;
		};

		void recordFitness(const Genome &genome, float base);
		std::vector<Vector3d> liveTargetCenters() const;
		std::vector<LiveTarget> liveTargets() const;
		std::vector<Vector3d> liveShotPositions() const;
		std::vector<LiveShot> liveShots() const;

		virtual void draw(sf::RenderTarget& target, DrawStates states) const;

		void killed();
		void missed();

		unsigned int get_kills() const { return kills; }
		unsigned int get_misses() const { return misses; }

		Population &evo() { return population; }
		const Population &evo() const { return population; }

		Archive &store() { return archive; }

		unsigned long long tickIndex() const { return ticks; }

		unsigned generation() const { return population.generationIndex(); }
	};


}
