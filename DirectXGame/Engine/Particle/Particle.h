#pragma once
#include "../Math/WorldTransformEx.h"
#include "KamataEngine.h"

namespace MyEngine {
/// <summary>
/// 全パーティクルの共通インターフェースおよび基本情報の定義。
/// ライフサイクル状態の保持 : パーティクルの生存期間（lifetime）、経過時間（age）、および有効フラグ（active）の管理。
/// 共通トランスフォームの提供 : 全ての派生パーティクルが使用する座標・回転・スケール情報の保持。
/// 更新インターフェースの抽象化 : 派生クラスごとに異なる挙動（移動、拡大、フェード等）を実装するための純粋仮想関数 Update の定義。
/// </summary>
class Particle {
public:
	virtual ~Particle() = default;

	bool active = false;
	float lifetime = 0.0f;
	float age = 0.0f;
	float alpha = 1.0f;

	WorldTransformEx transform;

	/// <summary>
	/// パーティクルの更新処理（純粋仮想関数）。
	/// 経過時間の加算、および有効フラグ（active）の管理を派生クラスで行う。
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間（秒）。物理計算やタイマー更新に使用する。</param>
	virtual void Update(float deltaTime) = 0;
};
}


