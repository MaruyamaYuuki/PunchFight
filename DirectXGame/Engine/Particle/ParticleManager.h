#pragma once
#include "Particle.h"

class ParticleManager {
public:
	virtual ~ParticleManager() = default;

	void Update(float deltaTime);
	virtual void Draw(KamataEngine::Camera& camera) = 0;

protected:
	std::vector<std::unique_ptr<Particle>> particles_;
};
