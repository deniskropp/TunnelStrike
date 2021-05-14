#pragma once

#include <SFML/Audio.hpp>

namespace TunnelStrike {

	class Sfx
	{
	public:
		static Sfx& instance()
		{
			static Sfx sfx;

			return sfx;
		}

	private:
		sf::SoundBuffer shot_buffer;
		sf::Sound shot_sound;
		sf::SoundBuffer hit_buffer;
		sf::Sound hit_sound;

	public:
		Sfx();

		void PlayShot();
		void PlayHit();
	};

}