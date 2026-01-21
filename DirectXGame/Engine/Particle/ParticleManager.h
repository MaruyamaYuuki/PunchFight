#pragma once
#include "Particle.h"

namespace MyEngine {
class ParticleManager {
public:
	virtual ~ParticleManager() = default;

	void Update(float deltaTime);
	virtual void Draw(KamataEngine::Camera& camera) = 0;

protected:
	std::vector<std::unique_ptr<Particle>> particles_;
};
} // namespace MyEngine