#include "SmokeParticleManager.h"

using namespace KamataEngine;

void SmokeParticleManager::Initialize() {
	model_ = Model::CreateFromOBJ("player", true);
	textureHandle_ = TextureManager::Load("effects/smoke.png");
}

void SmokeParticleManager::Spawn(const Vector3& pos) {
	auto p = std::make_unique<SmokeParticle>();
	p->active = true;
	p->age = 0.0f;
	p->lifetime = 0.5f;

	p->transform.Initialize();
	p->transform.translation_ = pos; // 敵の現在位置
	p->transform.scale_ = {0.4f, 0.4f, 0.4f};

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

		float scale = 0.4f + t * 0.3f;
		p->transform.scale_ = {scale, scale, scale};

		p->transform.translation_.y += 0.01f;
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