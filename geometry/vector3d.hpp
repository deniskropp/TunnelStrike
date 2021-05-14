#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <SFML/Graphics.hpp>

#include "../utils/tools.hpp"

#include <iostream>
#include <iomanip> // for std::setprecision and std::setw

namespace TunnelStrike {

	class Vector3d {
	private:
		double x, y, z;
		sf::Color color;

	public:
		// constructors
		Vector3d() : x(0.0), y(0.0), z(0.0), color(sf::Color::White) {}
		Vector3d(double _x, double _y, double _z) : x(_x), y(_y), z(_z), color(sf::Color::White) {}
		Vector3d(double _x, double _y, double _z, const sf::Color& _color) : x(_x), y(_y), z(_z), color(_color) {}
		Vector3d(const Vector3d& v) : x(v.x), y(v.y), z(v.z), color(v.color) {}
		Vector3d(const Vector3d& v, const sf::Color& _color) : x(v.x), y(v.y), z(v.z), color(_color) {}

		// operators
		Vector3d& operator=(const Vector3d& v);
		Vector3d operator+(const Vector3d& v) const;
		Vector3d operator+=(const Vector3d& v);
		Vector3d operator-(const Vector3d& v) const;
		Vector3d operator-=(const Vector3d& v);
		Vector3d operator-();
		Vector3d operator*(const double factor) const;
		Vector3d operator*=(const double factor);
		double operator*(const Vector3d& v) const;

		// others
		void set_color(const sf::Color& _color) { color = _color; }
		double norm() const { return sqrt(x * x + y * y + z * z); }
		void normalize() { *this = (*this) * (1 / this->norm()); }
		void rotate(const Vector3d& center, const Vector3d& axis, double theta);

		Vector3d get_normalized() const;
		double distance_to(const Vector3d& v) const;

		double get_x() const { return x; }
		double get_y() const { return y; }
		double get_z() const { return z; }

		friend std::ostream& operator<<(std::ostream& os, const Vector3d& v);
		friend class Segment3d;
		friend class Plane3d;
		friend class Solid3d;
		friend class Camera3d;
	};

}

#endif
