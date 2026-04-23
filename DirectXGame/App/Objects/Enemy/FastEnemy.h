#pragma once
#include "EnemyBase.h"

// 残像一つ分のデータ
struct GhostEffect {
	KamataEngine::Vector3 pos;
	float facingDir;
	float alpha;
	uint32_t textureHandle;
};

class FastEnemy : public EnemyBase {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	FastEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data">エネミーデータ</param>
	void Initialize(const EnemyData& data) override;

	/// 更新処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（追従対象）</param>
	/// <param name="allEnemies">全エネミーのリスト（他の敵との重なり回避計算に使用）</param>
	virtual void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies);

	void UpdateRetreat(float deltaTime);

	bool IsRetreatFinished() const;

private:
	void StartRetreat();

	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

private:
	MyEngine::GameConfigManager* cfg_ = nullptr;

	// --- ヒット＆アラン用 ---
	bool isRetreating_ = false;           // 離脱中フラグ
	float retreatTimer_ = 0.0f;           // 離脱時間タイマー
	float retreatDuration_ = 1.0f;        // 離脱する時間（秒）
	float retreatSpeedMultiplier_ = 1.5f; // 離脱時の速度倍率
};
