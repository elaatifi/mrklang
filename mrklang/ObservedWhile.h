#pragma once

#include <functional>

#include "Common.h"

namespace MRK {
	void ObservedWhile(mrks function<void(bool&)> loop);
}