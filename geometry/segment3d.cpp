#include "../utils/parameters.hpp"
#include "camera3d.hpp"
#include "segment3d.hpp"

namespace TunnelStrike {

	// ##############################################
	// ### operators ################################
	// ##############################################

	Segment3d& Segment3d::operator=(const Segment3d& s) {
		if (this != &s) {
			a = s.a;
			b = s.b;
		}

		return *this;
	}

	Segment3d Segment3d::operator+=(const Vector3d& v) {
		a += v;
		b += v;

		return *this;
	}

	void Segment3d::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		Segment3d ts = Camera3d::instance().transform_segment(*this);

		for (const auto& side : Camera3d::instance().frustrum) {
			if (side.handle_intersection_of_segment_with_plane(ts))
				return;
		}

		sf::VertexArray figure(sf::Lines, 2);

		figure[0] = sf::Vertex(Camera3d::instance().frustrum[0].get_projection_on_plane(ts.a, Parameters::window_width, Parameters::window_height));
		figure[1] = sf::Vertex(Camera3d::instance().frustrum[0].get_projection_on_plane(ts.b, Parameters::window_width, Parameters::window_height));

		target.draw(figure, states);
	}
}