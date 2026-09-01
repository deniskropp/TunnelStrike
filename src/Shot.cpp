#include "Shot.hpp"

#include <algorithm>


namespace TunnelStrike {

	namespace {

		double distance_point_segment(const Vector3d &p, const Vector3d &a, const Vector3d &b)
		{
			const Vector3d ab = b - a;
			const double ab2 = ab * ab;
			if (ab2 < 1e-12)
				return p.distance_to(a);

			const double t = std::clamp((p - a) * ab / ab2, 0.0, 1.0);
			return p.distance_to(a + ab * t);
		}

	}

	Shot::Shot(const Vector3d& pos, const Vector3d& dir)
		:
		pos(pos),
		last(pos),
		dir(dir)
	{
	}

	void Shot::Tick(sf::Time delta)
	{
		last = pos;
		pos += dir * delta.asSeconds() * 10.0f;
	}

	void Shot::draw(sf::RenderTarget& target, DrawStates states) const
	{
		Vector3d p = position();

		Segment3d s1(p, p + Vector3d(1.0f, 0.0f, 0.0f), sf::Color(255, 255, 255), sf::Color(255, 255, 255));
		Segment3d s2(p, p + Vector3d(0.0f, 1.0f, 0.0f), sf::Color(255, 255, 255), sf::Color(255, 255, 255));
		Segment3d s3(p, p + Vector3d(-1.0f, 0.0f, 0.0f), sf::Color(255, 255, 255), sf::Color(255, 255, 255));
		Segment3d s4(p, p + Vector3d(0.0f, -1.0f, 0.0f), sf::Color(255, 255, 255), sf::Color(255, 255, 255));

		s1.draw(target, states);
		s2.draw(target, states);
		s3.draw(target, states);
		s4.draw(target, states);
	}

	bool Shot::outsideWalls() const
	{
		Vector3d current = position();

		if (current.get_x() >= 10.0f || current.get_y() >= 10.0f || current.get_x() <= -10.0f || current.get_y() <= -10.0f)
			return true;

		return false;
	}

	std::shared_ptr<Target> Shot::hitTarget(std::list<std::shared_ptr<Target>> targets) const
	{
		Vector3d current = position();

		for (auto t : targets) {
			const double radius = 1.35 + static_cast<double>(t->genome().size) * 0.42;
			if (distance_point_segment(t->GetCenter(), last, current) < radius)
				return t;
		}

		return 0;
	}

	Vector3d Shot::position() const
	{
		return pos;
	}


}