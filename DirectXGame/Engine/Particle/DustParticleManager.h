#pragma once
#include "DustParticle.h"
#include "ParticleManager.h"

namespace MyEngine {
class DustParticleManager : public ParticleManager {
public:
	void Initialize();
	void Spawn(const KamataEngine::Vector3& pos);
	void Draw(KamataEngine::Camera& camera) override;

private:
	float RandomRange(float min, float max);

	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
} // namespace MyEngine