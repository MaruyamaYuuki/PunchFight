#pragma once
#include "Particle.h"

/// <summary>
/// 煙のパーティクル
/// </summary>
class SmokeParticle : public Particle {
public:
	KamataEngine::Vector3 initialScale;

	void Update(float deltaTime) override;
};