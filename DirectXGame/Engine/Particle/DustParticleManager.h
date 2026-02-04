#pragma once
#include "DustParticle.h"
#include "ParticleManager.h"

namespace MyEngine {
/// <summary>
/// 地面から舞い上がる粉塵（砂埃）演出の一括生成および管理。
/// ランダム性を伴う大量生成 : Spawn 関数において、単一の座標から複数（5〜10個）の DustParticle をランダムな速度・サイズ・寿命で放出し、自然な拡散演出を作る。
/// リソース管理 : 粉塵専用のテクスチャ（dust2.png）とビルボードモデルの保持。
/// 描画制御 : 各パーティクルが持つ個別のアルファ値をモデルに反映し、カメラに対して一括描画を行う。
/// </summary>
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