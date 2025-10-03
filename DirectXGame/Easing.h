#pragma once

#include "KamataEngine.h"
#include <cmath>

using namespace KamataEngine;

namespace Easing {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// EaseOutBack
inline float EaseOutBack(float t) {
	// t ∈ [0,1]
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	float p = t - 1.0f;
	return 1.0f + c3 * p * p * p + c1 * p * p;
}

// EaseInOutSine
inline float EaseInOutSine(float t) {
	// t ∈ [0,1]
	return -(std::cos(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f;
}

} // namespace Easing
