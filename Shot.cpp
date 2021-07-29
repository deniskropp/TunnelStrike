#include "Shot.hpp"


namespace TunnelStrike {


	Shot::Shot(const Vector3d& pos, const Vector3d& dir)
		:
		pos(pos),
		dir(dir)
	{
	}

	void Shot::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		Vector3d p = position(clock.getElapsedTime());

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
		Vector3d current = position(clock.getElapsedTime());

		if (current.get_x() >= 10.0f || current.get_y() >= 10.0f || current.get_x() <= -10.0f || current.get_y() <= -10.0f)
			return true;

		return false;
	}

	std::shared_ptr<Target> Shot::hitTarget(std::list<std::shared_ptr<Target>> targets) const
	{
		Vector3d current = position(clock.getElapsedTime());

		for (auto t : targets) {
			if (current.distance_to(t->GetCenter()) < 3.0f)
				return t;
		}

		return 0;
	}

	Vector3d Shot::position(sf::Time time) const
	{
		return pos + dir * time.asSeconds() * 10.0f;
	}


}