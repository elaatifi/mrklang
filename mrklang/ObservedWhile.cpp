#include "ObservedWhile.h"

namespace MRK {
	void ObservedWhile(mrks function<void(bool&)> loop) {
		bool run = true;
		while (run) {
			loop(run);
		}
	}
}