#pragma once
#include "Particle.h"

namespace MyEngine {
 /// <summary>
/// 煙のパーティクル
/// </summary>
class SmokeParticle : public Particle {
public:
	KamataEngine::Vector3 initialScale;

	void Update(float deltaTime) override;
};
}
