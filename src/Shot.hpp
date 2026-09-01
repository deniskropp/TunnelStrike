#pragma once

#include <list>
#include <memory>

#include "geometry/vector3d.hpp"
#include "sfml_compat.hpp"

#include "Target.hpp"


namespace TunnelStrike {


	class Shot : public sf::Drawable
	{
	private:
		Vector3d pos;
		Vector3d last;
		Vector3d dir;

	public:
		Shot(const Vector3d& pos, const Vector3d& dir);

		void Tick(sf::Time delta);

		virtual void draw(sf::RenderTarget& target, DrawStates states) const;

		bool outsideWalls() const;

		std::shared_ptr<Target> hitTarget(std::list<std::shared_ptr<Target>> targets) const;

		Vector3d GetCenter() const { return position(); }

		Vector3d GetDir() const { return dir; }

		friend std::ostream& operator << (std::ostream& os, const Shot& shot)
		{
			os << shot.position();

			return os;
		}

	private:
		Vector3d position() const;
	};


}