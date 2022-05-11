#include "segmented.hpp"

#include "../utils/parameters.hpp"


namespace TunnelStrike {

	Segmented::Segmented()
	{
	}

	void Segmented::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		sf::VertexArray figure(sf::Lines);

		for (auto s : segments) {
			bool outside_frustrum = false;

			s = Camera3d::instance().transform_segment(s);

			for (const auto& side : Camera3d::instance().frustrum) {
				outside_frustrum = side.handle_intersection_of_segment_with_plane(s);
				if (outside_frustrum)
					break;
			}

			if (!outside_frustrum) {
				figure.append(Camera3d::instance().frustrum[0].get_projection_on_plane(s.a, Parameters::window_width, Parameters::window_height));
				figure.append(Camera3d::instance().frustrum[0].get_projection_on_plane(s.b, Parameters::window_width, Parameters::window_height));
			}
		}

		target.draw(figure, states);
	}
}