#pragma once
#include "Particle.h"

namespace MyEngine {
/// <summary>
/// エフェクトの効率的な生成と描画管理。
/// 大量のパーティクルインスタンスの生成・更新・破棄の管理。
/// 発生源となるオブジェクト（エネミー等）が消滅した後も、エフェクトが寿命まで独立して生存し続ける処理。
/// </summary>
class ParticleManager {
public:
	virtual ~ParticleManager() = default;

	/// <summary>
	/// 全パーティクルの状態更新。
	/// 寿命が尽きた（active == false）パーティクルをリストから除外する。
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間（秒）</param>
	void Update(float deltaTime);

	/// <summary>
	/// 管理下の全パーティクルの一括描画（純粋仮想関数）。
	/// </summary>
	/// <param name="camera">描画に使用するビュープロジェクション変換用のカメラ</param>
	virtual void Draw(KamataEngine::Camera& camera) = 0;

protected:
	std::vector<std::unique_ptr<Particle>> particles_;
};
} // namespace MyEngine