#pragma once

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>

namespace TunnelStrike {

	// SFML 3 takes const RenderStates&; SFML 2 takes RenderStates by value.
#if SFML_VERSION_MAJOR >= 3
	using DrawStates = const sf::RenderStates&;
#else
	using DrawStates = sf::RenderStates;
#endif

	inline sf::VideoMode make_video_mode(unsigned width, unsigned height)
	{
#if SFML_VERSION_MAJOR >= 3
		return sf::VideoMode({width, height});
#else
		return sf::VideoMode(width, height);
#endif
	}

}
