#pragma once
#include "Particle.h"

namespace MyEngine {
 /// <summary>
/// キャラクターの移動や着地時に発生する「粉塵」の挙動実装。
/// 上昇・拡散移動の制御 : 設定された速度（velocityX,velocityY）に基づき、ふわっと上昇しながら横に広がる物理挙動の実行。
/// 時間経過による透過度制御 : 寿命が尽きるに従って直線的にフェードアウト（alphaの減衰）させる視覚演出の管理。
/// </summary>
class DustParticle : public Particle {
public:
	float velocityX = 0.0f;
	float velocityY = 0.02f;

	/// <summary>
	/// 粉塵パーティクルの挙動更新。
	/// 速度に基づいた座標移動と、生存期間に応じたフェードアウト（Alpha値の計算）を実行する。
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間（秒）</param>
	void Update(float deltaTime) override;
};
}

