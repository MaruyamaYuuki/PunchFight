#include "SmokeParticle.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

namespace MyEngine {
void SmokeParticle::Update(float deltaTime) {
	if (!active)
		return;

	age += deltaTime;
	if (age >= lifetime) {
		active = false;
		return;
	}

	float t = age / lifetime;
	alpha = 1.0f - t;

	// ---- 初期サイズに対して拡大していく ----
	float scaleRate = 1.0f + t * 0.5f; // 50%大きくなる場合
	transform.scale_ = initialScale * scaleRate;

	transform.translation_.y += 0.005f;
	transform.UpdateMatrix();
}
} // namespace MyEngine