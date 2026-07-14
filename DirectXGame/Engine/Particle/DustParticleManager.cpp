#include "DustParticleManager.h"
#include <random>

using namespace KamataEngine;

namespace MyEngine {
void DustParticleManager::Initialize() {
	model_.reset(Model::CreateFromOBJ("quad", true));

	// 粉塵用テクスチャを読み込む
	textureHandle_ = TextureManager::Load("effects/dust2.png");
}

void DustParticleManager::Spawn(const Vector3& pos) {

	int32_t count = 5 + (int32_t)RandomRange(0, 5); // 5〜10個ランダム

	for (int32_t i = 0; i < count; i++) {
		auto p = std::make_unique<DustParticle>();
		p->active = true;
		p->age = 0.0f;
		p->lifetime = RandomRange(0.3f, 0.6f); // 寿命もランダムに

		p->transform.Initialize();
		p->transform.translation_ = pos;

		// ランダムな大きさ
		float s = RandomRange(0.05f, 0.1f);
		p->transform.scale_ = {s, s, s};

		// ランダム方向に散らばる
		p->velocityX = RandomRange(-0.03f, 0.03f);
		p->velocityY = RandomRange(0.03f, 0.08f);

		particles_.push_back(std::move(p));
	}
}

float DustParticleManager::RandomRange(float min, float max) {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}

void DustParticleManager::Draw(Camera& camera) {
	for (auto& p : particles_) {
		if (!p->active)
			continue;

		model_->SetAlpha(p->alpha);
		model_->Draw(p->transform, camera, textureHandle_);
	}
}
} // namespace MyEngine