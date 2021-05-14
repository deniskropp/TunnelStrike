#ifndef LOOPTIMER_HPP
#define LOOPTIMER_HPP

#include <SFML/Graphics.hpp>

namespace TunnelStrike {

	class LoopTimer : public sf::Clock {
	private:
		sf::Time loop_time;

	public:
		LoopTimer(const sf::Time& _loop_time) : loop_time(_loop_time) {}

		bool is_done();
	};

}

#endif
