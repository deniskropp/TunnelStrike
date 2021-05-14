#include "camera3d.hpp"
#include "../utils/parameters.hpp"


#define CAMERA_ROTATION_SENSIBILITY    0.25
#define CAMERA_TRANSLATION_SENSIBILITY 4


namespace TunnelStrike {

	// ##############################################
	// ### constructors #############################
	// ##############################################

	// #TODO: explain frustrum creation
	Camera3d::Camera3d(const Vector3d& _position, double _theta_x, double _theta_y, double _theta_z)
		:
		position(_position),
		theta_x(as_radians(_theta_x)),
		theta_y(as_radians(_theta_y)),
		theta_z(as_radians(_theta_z))
	{
		const double horizontal_angle = atan2(Parameters::window_width / 2.0, PROJECTION_FACTOR) - 0.0001;
		const double vertical_angle = atan2(Parameters::window_height / 2.0, PROJECTION_FACTOR) - 0.0001;
		const double ch = cos(horizontal_angle);
		const double sh = sin(horizontal_angle);
		const double cv = cos(vertical_angle);
		const double sv = sin(vertical_angle);

		frustrum[0] = Plane3d(Vector3d(0, 0, 0), Vector3d(0, 0, 1)); // close
		frustrum[1] = Plane3d(Vector3d(0, 0, PROJECTION_MAX_DEPTH), Vector3d(0, 0, -1)); // far
		frustrum[2] = Plane3d(Vector3d(0, 0, 0), Vector3d(ch, 0, sh)); // left
		frustrum[3] = Plane3d(Vector3d(0, 0, 0), Vector3d(-ch, 0, sh)); // right
		frustrum[4] = Plane3d(Vector3d(0, 0, 0), Vector3d(0, cv, sv)); // top
		frustrum[5] = Plane3d(Vector3d(0, 0, 0), Vector3d(0, -cv, sv)); // bottom
	}


	// ##############################################
	// ### operators ################################
	// ##############################################

	Camera3d Camera3d::operator+=(const Vector3d& v) {
		position += v * CAMERA_TRANSLATION_SENSIBILITY;

		return *this;
	}


	// ##############################################
	// ### others ###################################
	// ##############################################

	void Camera3d::reload_frustrum() {
		*this = Camera3d(position, as_degrees(theta_x), as_degrees(theta_y), as_degrees(theta_z));
	}

	void Camera3d::rotate(const double mouse_move_x, const double mouse_move_y) {
		theta_x -= as_radians(mouse_move_y) * CAMERA_ROTATION_SENSIBILITY;
		theta_y += as_radians(mouse_move_x) * CAMERA_ROTATION_SENSIBILITY;
	}

	// #TODO: missing comment
	void Camera3d::move(const DIRECTION direction) {
		Vector3d normal(cos(theta_x) * sin(theta_y), -sin(theta_x), cos(theta_x) * cos(theta_y));
		normal.normalize();

		Vector3d orthog(normal.z, 0, -normal.x);
		orthog.normalize();

		Vector3d up(0, 1, 0);

		switch (direction) {
		case DIRECTION::FRONT:
			*this += normal;
			break;
		case DIRECTION::BACK:
			*this += -normal;
			break;
		case DIRECTION::RIGHT:
			*this += orthog;
			break;
		case DIRECTION::LEFT:
			*this += -orthog;
			break;
		case DIRECTION::UP:
			*this += up;
			break;
		case DIRECTION::DOWN:
			*this += -up;
			break;
		}
	}

	void Camera3d::translate(Vector3d delta)
	{
		position += delta;
	}

	// #TODO: missing comment
	Vector3d Camera3d::transform_vector(const Vector3d& v) const {
		double cx = cos(theta_x), cy = cos(theta_y), cz = cos(theta_z);
		double sx = sin(theta_x), sy = sin(theta_y), sz = sin(theta_z);
		Vector3d u = v - position; // camera as the center of the world in (0, 0, 0)

		double x = u.x, y = u.y, z = u.z;

		// rotations
		return (Vector3d(cy * (sz * y + cz * x) - sy * z,
			sx * (cy * z + sy * (sz * y + cz * x)) + cx * (cz * y - sz * x),
			cx * (cy * z + sy * (sz * y + cz * x)) - sx * (cz * y - sz * x), v.color));
	}

	Segment3d Camera3d::transform_segment(const Segment3d& s) const {
		return Segment3d(transform_vector(s.a), transform_vector(s.b));
	}

}