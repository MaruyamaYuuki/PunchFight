#pragma once
#include "Particle.h"

namespace MyEngine {
 /// <summary>
/// 粉塵パーティクル
/// </summary>
class DustParticle : public Particle {
public:
	float velocityX = 0.0f;
	float velocityY = 0.02f;

	void Update(float deltaTime) override;
};
}

