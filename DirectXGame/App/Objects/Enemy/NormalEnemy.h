#pragma once
#include "EnemyBase.h"

/// <summary>
/// 通常敵
/// </summary>
class NormalEnemy : public EnemyBase {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	NormalEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data">エネミーデータ</param>
	void Initialize(const EnemyData& data) override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	void Update(const KamataEngine::Vector3& playerPos) override;

private:
	float attackTimer_ = 0.0f;
	bool isAttacking_ = false;
};
