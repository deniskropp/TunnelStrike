#pragma once

#include <array>

#include <SFML/Graphics.hpp>

#include "geometry/segmented.hpp"


namespace TunnelStrike {


	class Quad : public Segmented
	{
	private:
		std::array<sf::Vector3f,4> v;
		std::array<sf::Color,4> color;

	public:
		Quad(sf::Vector3f a, sf::Vector3f b, sf::Vector3f c, sf::Vector3f d, sf::Color ca, sf::Color cb, sf::Color cc, sf::Color cd);

	private:
		Segment3d getSegment(int v1, int v2) const;
	};


}