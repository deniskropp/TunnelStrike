#include "plane3d.hpp"

namespace TunnelStrike {

	// ##############################################
	// ### operators ################################
	// ##############################################

	Plane3d& Plane3d::operator=(const Plane3d& p) {
		if (this != &p) {
			base = p.base;
			normal = p.normal;
		}

		return *this;
	}


	// ##############################################
	// ### others ###################################
	// ##############################################

	// p(x, y, z) in plane [n(a, b, c), b] ⟺ n . (p - b)        = 0
	//                                     ⟺ n.p - n.b          = 0
	//                                     ⟺ ax + by + cz - n.b = 0 ⟹ d = - n.b
	double Plane3d::get_equation_coefficient_d() const {
		return -(normal * base);
	}

	// v(x, y, z) point and p plane [n, b] with n unit vector, v2 normal intersection of v with p, distance:
	// D = | n . (v - v2) |
	//   = | n.v - n.v2 |
	//   = | n.v - d | because d = - n.v2
	// if p[N, b], D = | n.v - d | * 1 / | n |
	// removing the | | of the nominator yields the "signed distance", eg:
	//    - the distance D
	//    - the sign being > 0 if the point is "above the normal side", < 0 if "under the normal side"
	double Plane3d::get_signed_distance_from_point_to_plane(const Vector3d& v) const {
		return v * normal + get_equation_coefficient_d() / normal.norm();
	}

	// #TODO: missing comment
	bool Plane3d::handle_intersection_of_segment_with_plane(Segment3d& s) const {
		const double da = get_signed_distance_from_point_to_plane(s.a);
		const double db = get_signed_distance_from_point_to_plane(s.b);

		if (da < 0 && db < 0)
			return true;
		else if (da > 0 && db < 0) {
			double f = da / (da - db); // intersection factor (between 0 and 1)
			sf::Color new_color = sf::Color(s.a.color.r + (std::uint8_t)(f * (s.b.color.r - s.a.color.r)),
											s.a.color.g + (std::uint8_t)(f * (s.b.color.g - s.a.color.g)),
											s.a.color.b + (std::uint8_t)(f * (s.b.color.b - s.a.color.b)));
			s.b = s.a + (s.b - s.a) * f;

			s.b.color = new_color;
		}
		else if (da < 0 && db > 0) {
			std::swap(s.a, s.b);
			handle_intersection_of_segment_with_plane(s);
		}

		return false;
	}

	// #TODO: explain change point opacity
	sf::Vertex Plane3d::get_projection_on_plane(const Vector3d& v, const unsigned window_width, const unsigned window_height) const {
		sf::Color vertex_color = v.color;

		vertex_color.a = (std::uint8_t)(map(get_signed_distance_from_point_to_plane(v), 0, PROJECTION_MAX_DEPTH, 255, 0));

		return sf::Vertex(sf::Vector2f((float)(PROJECTION_FACTOR * v.x / v.z) + window_width / 2,
									   (float)(PROJECTION_FACTOR * v.y / v.z) + window_height / 2),
									   vertex_color);
	}

}