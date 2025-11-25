#include "DustParticleManager.h"
#include <random>

using namespace KamataEngine;

void DustParticleManager::Initialize() {
	model_ = Model::CreateFromOBJ("player", true);

	// 粉塵用テクスチャを読み込む
	textureHandle_ = TextureManager::Load("effects/smoke.png");
}

void DustParticleManager::Spawn(const Vector3& pos) {
	auto p = std::make_unique<DustParticle>();
	p->active = true;
	p->age = 0.0f;
	p->lifetime = 0.4f;

	p->transform.Initialize();
	p->transform.translation_ = pos;
	p->transform.scale_ = {0.2f, 0.2f, 0.2f};

	// ランダムに横に飛ぶ感じ（粉っぽさ）
	p->velocityX = RandomRange(-0.01f, 0.01f);
	p->velocityY = 0.02f;

	particles_.push_back(std::move(p));
}

void DustParticleManager::Update(float deltaTime) {
	for (auto& p : particles_) {
		if (!p->active)
			continue;

		p->age += deltaTime;

		// 寿命
		if (p->age >= p->lifetime) {
			p->active = false;
			continue;
		}

		float t = p->age / p->lifetime;

		// フェードアウト
		p->alpha = 1.0f - t;

		// 少し大きくなる
		float scale = 0.3f + t * 0.2f;
		p->transform.scale_ = {scale, scale, scale};

		// ふわっと上昇しつつ横に広がる
		p->transform.translation_.x += p->velocityX;
		p->transform.translation_.y += p->velocityY;

		p->transform.UpdateMatrix();
	}
}

void DustParticleManager::Draw(Camera& camera) {
	for (auto& p : particles_) {
		if (!p->active)
			continue;

		model_->SetAlpha(p->alpha);
		model_->Draw(p->transform, camera, textureHandle_);
	}
}

float DustParticleManager::RandomRange(float min, float max) {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}
