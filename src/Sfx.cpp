#include "Sfx.hpp"

namespace TunnelStrike
{

	Sfx::Sfx()
	{
		if (!shot_buffer.loadFromFile("data/shot.wav"))
			throw std::runtime_error("Failed to load shot.wav");

		shot_sound.setBuffer(shot_buffer);

		if (!hit_buffer.loadFromFile("data/hit.wav"))
			throw std::runtime_error("Failed to load hit.wav");
		
		hit_sound.setBuffer(hit_buffer);

		if (!die_buffer.loadFromFile("data/die.wav"))
			throw std::runtime_error("Failed to load die.wav");
		
		die_sound.setBuffer(die_buffer);
	}

	void Sfx::PlayShot()
	{
		shot_sound.play();
	}

	void Sfx::PlayHit()
	{
		hit_sound.play();
	}

	void Sfx::PlayDie()
	{
		die_sound.play();
	}
}