#pragma once
#include "Particle.h"

/// <summary>
/// エフェクトの効率的な生成と描画管理。
/// 大量のパーティクルインスタンスの生成・更新・破棄の管理。
/// 発生源となるオブジェクト（エネミー等）が消滅した後も、エフェクトが寿命まで独立して生存し続ける処理。
/// </summary>
namespace MyEngine {
class ParticleManager {
public:
	virtual ~ParticleManager() = default;

	void Update(float deltaTime);
	virtual void Draw(KamataEngine::Camera& camera) = 0;

protected:
	std::vector<std::unique_ptr<Particle>> particles_;
};
} // namespace MyEngine