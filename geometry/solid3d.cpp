#include "solid3d.hpp"

namespace TunnelStrike {

	// ##############################################
	// ### operators ################################
	// ##############################################

	Solid3d Solid3d::operator+=(const Solid3d& solid) {
		for (const auto& s : solid.edges)
			this->add_segment(s);

		return *this;
	}

	Solid3d Solid3d::operator+(const Vector3d& v) const {
		Solid3d new_solid(*this);

		for (auto& s : new_solid.edges)
			s += v;

		new_solid.center += v;

		return new_solid;
	}

	Solid3d Solid3d::operator+=(const Vector3d& v) {
		for (auto& s : edges)
			s += v;

		center += v;

		return *this;
	}


	// ##############################################
	// ### others ###################################
	// ##############################################

	void Solid3d::set_center(const Vector3d& _center) {
		*this += _center;
		center = _center;
	}

	// #TODO: missing comment
	void Solid3d::render_solid(sf::RenderWindow& window, const unsigned window_width, const unsigned window_height, const Camera3d& camera) {
		sf::VertexArray figure(sf::PrimitiveType::Lines);

		for (auto s : edges) {
			bool outside_frustrum = false;
			
			s = camera.transform_segment(s);

			for (const auto& side : camera.frustrum) {
				outside_frustrum = side.handle_intersection_of_segment_with_plane(s);
				if (outside_frustrum)
					break;
			}

			if (!outside_frustrum) {
				figure.append(camera.frustrum[0].get_projection_on_plane(s.a, window_width, window_height));
				figure.append(camera.frustrum[0].get_projection_on_plane(s.b, window_width, window_height));
			}
		}

		window.draw(figure);
	}

	void Solid3d::rotate(const Vector3d& rotation_center, const Vector3d& axis, const double theta, const bool object_axis) {

		Vector3d center_of_rotation(rotation_center);

		if (object_axis)
			center_of_rotation = center;

		for (auto& s : edges) {
			s.a.rotate(center_of_rotation, axis, theta);
			s.b.rotate(center_of_rotation, axis, theta);
		}

		center.rotate(center_of_rotation, axis, theta);
	}

}