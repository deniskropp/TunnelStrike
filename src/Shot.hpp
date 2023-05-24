#pragma once

#include <list>
#include <memory>

#include "geometry/vector3d.hpp"

#include "Target.hpp"


namespace TunnelStrike {


	class Shot : public sf::Drawable
	{
	private:
		Vector3d pos;
		Vector3d dir;
		sf::Clock clock;

	public:
		Shot(const Vector3d& pos, const Vector3d& dir);

		virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const;

		bool outsideWalls() const;

		std::shared_ptr<Target> hitTarget(std::list<std::shared_ptr<Target>> targets) const;

		Vector3d GetCenter() const { return pos; }

		friend std::ostream& operator << (std::ostream& os, const Shot& shot)
		{
			os << shot.position(shot.clock.getElapsedTime());

			return os;
		}

	private:
		Vector3d position(sf::Time time) const;
	};


}