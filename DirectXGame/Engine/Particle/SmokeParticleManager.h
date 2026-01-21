#pragma once
#include "ParticleManager.h"
#include "SmokeParticle.h"

namespace MyEngine {
class SmokeParticleManager : public ParticleManager {
public:
	void Initialize();
	void Spawn(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& size);
	void Draw(KamataEngine::Camera& camera) override;

	/// <summary>
	/// TextureHandleを設定する
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetTexture(uint32_t textureHandle) { textureHandle_ = textureHandle; }

private:
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
} // namespace MyEngine