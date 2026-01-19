#include "SmokeParticleManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void SmokeParticleManager::Initialize() {
	model_ = Model::CreateFromOBJ("quad", true);
	textureHandle_ = TextureManager::Load("effects/smoke.png");
}

void SmokeParticleManager::Spawn(const Vector3& pos, const Vector3& size) {
	auto p = std::make_unique<SmokeParticle>();

	p->active = true;
	p->age = 0.0f;
	p->lifetime = 0.5f;

	p->transform.Initialize();
	p->transform.translation_ = pos;
	p->transform.translation_.z += 0.1f;

	p->initialScale = size;
	p->transform.scale_ = size;

	particles_.push_back(std::move(p));
}

void SmokeParticleManager::Draw(Camera& camera) {
	for (auto& p : particles_) {
		if (!p->active)
			continue;

		model_->SetAlpha(p->alpha);
		model_->Draw(p->transform, camera, textureHandle_);
	}
}
