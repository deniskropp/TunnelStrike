#include "Walls.hpp"


namespace TunnelStrike {


	Walls::Walls(World& world)
		:
		Entity(world),
		maxz(0.0f)
	{
		if (!shader.loadFromFile("wall.vert", "wall.frag"))
			throw std::runtime_error("shader loading failed");
	}

	void Walls::Tick(sf::Time delta)
	{
		float z = (float)Camera3d::instance().center().get_z();

		if (z >= maxz) {
			q.clear();

			for (int i = 0; i < 4; i++) {
				q.push_back(Quad(sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f),
					sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f),
					sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
					sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60)));
				q.push_back(Quad(sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f),
					sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f),
					sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
					sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60)));
				q.push_back(Quad(sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f),
					sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f),
					sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
					sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60)));
				q.push_back(Quad(sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f),
					sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f),
					sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
					sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
					sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60)));
			}

			maxz += 100.0f;
		}
	}

	void Walls::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		sf::RenderStates s(states);

		s.shader = &shader;

		// draw walls
		for (auto& w : q)
			target.draw(w, s);
	}


}