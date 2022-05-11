#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "camera3d.hpp"
#include "segment3d.hpp"

namespace TunnelStrike {

	class Segmented : public sf::Drawable
	{
	protected:
		std::vector<Segment3d> segments;

	public:
		Segmented();

		virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;
	};

}