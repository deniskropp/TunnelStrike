#pragma once

#include <random>

#include "evo/Genome.hpp"
#include "geometry/segmented.hpp"

namespace TunnelStrike {

	struct Sense
	{
		Vector3d player;
		Vector3d nearest_shot;
		bool has_shot = false;
	};

	class Target : public Segmented
	{
	private:
		Vector3d center;
		float size;
		sf::Color color;
		Vector3d direction;
		Genome genome_;
		float lived = 0.0f;
		std::mt19937 rng_;

		void generate();

	public:
		Target(sf::Vector3f center, const Genome &genome);

		Vector3d GetCenter() const { return center; }

		const Genome &genome() const { return genome_; }

		void Act(sf::Time delta, const Sense &sense);

		int bodySegmentCount() const { return static_cast<int>(segments.size()); }

		float livedSeconds() const { return lived; }

		float fitnessIfKilled() const { return lived * 0.35f; }

		float fitnessIfSurvived() const { return lived * 1.15f + 8.0f; }
	};


}
