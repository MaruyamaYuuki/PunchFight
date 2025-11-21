#pragma once
#include "KamataEngine.h"
#include "SmokeParticle.h"

class SmokeParticleManager {
public:

	SmokeParticleManager() = default;

	~SmokeParticleManager() = default;

	void Initialize();

	void Spawn(const KamataEngine::Vector3& pos);

	void Update(float deltaTime);

	void Draw(KamataEngine::Camera& camera);

private:
	std::vector<std::unique_ptr<SmokeParticle>> particles_;

	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
