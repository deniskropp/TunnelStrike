#include "looptimer.hpp"

namespace TunnelStrike {

	bool LoopTimer::is_done() {
		if (getElapsedTime() >= loop_time) {
			this->restart();

			return true;
		}

		return false;
	}

}