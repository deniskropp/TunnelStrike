#ifndef SEGMENT_3D_HPP
#define SEGMENT_3D_HPP

#include <SFML/Graphics.hpp>
#include "vector3d.hpp"

namespace TunnelStrike {

	class Camera3d;

	class Segment3d : public sf::Drawable
	{
	private:
		Vector3d a, b;

	public:
		// constructors
		Segment3d() : a(Vector3d()), b(Vector3d()) {}
		Segment3d(const Vector3d& _a, const Vector3d& _b) : a(_a), b(_b) {}
		Segment3d(const Vector3d& _a, const Vector3d& _b, const sf::Color _color_a, const sf::Color _color_b) : a(_a, _color_a), b(_b, _color_b) {}
		Segment3d(const Segment3d& s) : a(s.a), b(s.b) {}

		// operators
		Segment3d& operator=(const Segment3d& s);
		Segment3d operator+=(const Vector3d& v);

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;


		friend class Plane3d;
		friend class Solid3d;
		friend class Camera3d;
		friend class Segmented;
	};

}

#endif
