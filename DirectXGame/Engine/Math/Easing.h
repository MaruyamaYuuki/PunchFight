#pragma once
#include "KamataEngine.h"
#include <cmath>

using namespace KamataEngine;

/// <summary>
/// 各種イージング関数を提供する名前空間
/// </summary>
namespace Easing {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// <summary>
/// EaseOutBack（加速してからオーバーシュートする補間）
/// </summary>
/// <param name="t">補間係数（0.0f～1.0f）</param>
/// <returns>補間後の値</returns>
inline float EaseOutBack(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	float p = t - 1.0f;
	return 1.0f + c3 * p * p * p + c1 * p * p;
}

/// <summary>
/// EaseInQuad（二乗の加速補間）
/// </summary>
/// <param name="t">補間係数（0.0f～1.0f）</param>
/// <returns>補間後の値</returns>
inline float EaseInQuad(float t) { return t * t; }

/// <summary>
/// EaseInOutSine（緩やかな加減速）
/// </summary>
/// <param name="t">補間係数（0.0f～1.0f）</param>
/// <returns>補間後の値</returns>
inline float EaseInOutSine(float t) { return -(std::cos(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f; }

/// <summary>
/// EaseOutBounce（バウンドするような減速）
/// </summary>
/// <param name="t">補間係数（0.0f～1.0f）</param>
/// <returns>補間後の値</returns>
inline float EaseOutBounce(float t) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1.0f / d1) {
		return n1 * t * t;
	} else if (t < 2.0f / d1) {
		t -= 1.5f / d1;
		return n1 * t * t + 0.75f;
	} else if (t < 2.5f / d1) {
		t -= 2.25f / d1;
		return n1 * t * t + 0.9375f;
	} else {
		t -= 2.625f / d1;
		return n1 * t * t + 0.984375f;
	}
}

} // namespace Easing
