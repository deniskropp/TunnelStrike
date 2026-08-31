#pragma once

#include <SFML/System.hpp>

namespace TunnelStrike {

	class SimClock
	{
	public:
		static constexpr float STEP_SECONDS = 0.03f;
		static constexpr int MAX_STEPS_PER_FRAME = 8;

		sf::Time step() const
		{
			return sf::seconds(STEP_SECONDS);
		}

		template <typename TickFn>
		int absorb(sf::Time frame_delta, TickFn &&tick)
		{
			acc += frame_delta;

			int steps = 0;
			const sf::Time quantum = step();

			while (acc >= quantum && steps < MAX_STEPS_PER_FRAME) {
				tick(quantum);
				acc -= quantum;
				++steps;
			}

			if (steps == MAX_STEPS_PER_FRAME)
				acc = sf::Time::Zero;

			return steps;
		}

		sf::Time remainder() const { return acc; }

		unsigned long long totalSteps() const { return total_steps; }

		void noteSteps(int n)
		{
			if (n > 0)
				total_steps += static_cast<unsigned long long>(n);
		}

	private:
		sf::Time acc;
		unsigned long long total_steps = 0;
	};

}
