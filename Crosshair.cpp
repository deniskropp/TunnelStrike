#include "Crosshair.hpp"

#include "utils/parameters.hpp"


namespace TunnelStrike {


	Crosshair::Crosshair(World& world)
		:
		Entity(world),
		v(sf::PrimitiveType::Lines)
	{
		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f, Parameters::window_height / 2.0f - 20.0f), sf::Color(111, 111, 111)));
		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f, Parameters::window_height / 2.0f - 5.0f), sf::Color(255, 255, 255)));

		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f - 20.0f, Parameters::window_height / 2.0f), sf::Color(111, 111, 111)));
		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f - 5.0f, Parameters::window_height / 2.0f), sf::Color(255, 255, 255)));

		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f, Parameters::window_height / 2.0f + 5.0f), sf::Color(255, 255, 255)));
		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f, Parameters::window_height / 2.0f + 20.0f), sf::Color(111, 111, 111)));

		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f + 20.0f, Parameters::window_height / 2.0f), sf::Color(111, 111, 111)));
		v.append(sf::Vertex(sf::Vector2f(Parameters::window_width / 2.0f + 5.0f, Parameters::window_height / 2.0f), sf::Color(255, 255, 255)));
	}

	void Crosshair::Tick(sf::Time delta)
	{
	}

	void Crosshair::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		target.draw(v, states);
	}


}