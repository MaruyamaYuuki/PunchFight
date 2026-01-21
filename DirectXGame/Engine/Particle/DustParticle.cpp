#include "DustParticle.h"

using namespace KamataEngine;

namespace MyEngine {
void DustParticle::Update(float deltaTime) {
	if (!active)
		return;

	age += deltaTime;

	// 寿命
	if (age >= lifetime) {
		active = false;
		return;
	}

	float t = age / lifetime;

	// フェードアウト
	alpha = 1.0f - t;

	// ふわっと上昇しつつ横に広がる
	transform.translation_.x += velocityX;
	transform.translation_.y += velocityY;

	transform.UpdateMatrix();
}
} // namespace MyEngine