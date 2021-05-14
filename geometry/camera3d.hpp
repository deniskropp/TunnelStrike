#pragma once

#include "../utils/tools.hpp"
#include "plane3d.hpp"
#include "segment3d.hpp"
#include "vector3d.hpp"


namespace TunnelStrike {


	class Camera3d {
	private:
		friend class Segment3d;
		friend class Segmented;
		friend class Solid3d;

	public:
		enum class DIRECTION { FRONT, BACK, RIGHT, LEFT, UP, DOWN };

		static Camera3d& instance()
		{
			static Camera3d camera;

			return camera;
		}

	private:
		Vector3d position;
		double theta_x, theta_y, theta_z;

		Plane3d frustrum[6];

	public:
		// constructors
		Camera3d(const Vector3d& _position = Vector3d(), double _theta_x = 0.0, double _theta_y = 0.0, double _theta_z = 0.0);

		// operators
		Camera3d operator+=(const Vector3d& v);

		// others
		void reload_frustrum();
		void rotate(const double mouse_move_x, const double mouse_move_y);
		void move(const DIRECTION direction);
		void translate(Vector3d delta);
		Vector3d transform_vector(const Vector3d& v) const;
		Segment3d transform_segment(const Segment3d& s) const;

		Vector3d center() const { return position; }
	};


}
