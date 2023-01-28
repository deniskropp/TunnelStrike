#include "Sfx.hpp"

namespace TunnelStrike
{

	Sfx::Sfx()
	{
		shot_buffer.loadFromFile("data/shot.wav");
		shot_sound.setBuffer(shot_buffer);

		hit_buffer.loadFromFile("data/hit.wav");
		hit_sound.setBuffer(hit_buffer);

		die_buffer.loadFromFile("data/die.wav");
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