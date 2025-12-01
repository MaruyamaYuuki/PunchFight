#include "SmokeParticleManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void SmokeParticleManager::Initialize() {
	model_ = Model::CreateFromOBJ("player", true);
	textureHandle_ = TextureManager::Load("effects/smoke.png");
}

void SmokeParticleManager::Spawn(const Vector3& pos, const KamataEngine::Vector3 size) {
	auto p = std::make_unique<SmokeParticle>();
	p->active = true;
	p->age = 0.0f;
	p->lifetime = 0.5f;

	p->transform.Initialize();
	p->transform.translation_ = pos; // 敵の現在位置
	p->transform.translation_.z += 0.1f;
	p->transform.scale_ = size;
	p->initialScale = size;

	particles_.push_back(std::move(p));
}

void SmokeParticleManager::Update(float deltaTime) {
	for (auto& p : particles_) {
		if (!p->active)
			continue;

		p->age += deltaTime;
		if (p->age >= p->lifetime) {
			p->active = false;
			continue;
		}

		float t = p->age / p->lifetime;
		p->alpha = 1.0f - t;

        // ---- 初期サイズに対して拡大していく ----
		float scaleRate = 1.0f + t * 0.5f; // 50%大きくなる場合
		p->transform.scale_ = p->initialScale * scaleRate;

		p->transform.translation_.y += 0.005f;
		p->transform.UpdateMatrix();
	}
}

void SmokeParticleManager::Draw(Camera& camera) {
	for (auto& p : particles_) {
		if (!p->active)
			continue;

		model_->SetAlpha(p->alpha);
		model_->Draw(p->transform, camera, textureHandle_);
	}
}