// BossEnemy.h
#pragma once
#include "EnemyBase.h"

/// <summary>
/// ボス専用クラス
/// ・通常攻撃
/// ・3連続タックル攻撃
/// </summary>
class BossEnemy : public EnemyBase {
public:
	void Initialize(const EnemyData& data) override;
	void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) override;

private:
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

	// 3連続タックル
	void TripleTackleAttack();

	// 攻撃開始時の初期化
	void EnterAttackMode(const KamataEngine::Vector3& playerPos);

private:
	MyEngine::GameConfigManager* cfg_ = nullptr;

	// ===== タックル設定 =====
	bool useTripleTackle_ = true; // 必要なら確率で切り替え可能

	bool isTackleCharging_ = false;
	bool isTackling_ = false;

	float tackleChargeTimer_ = 0.0f;
	float tackleChargeTime_ = 0.0f;

	float tackleMoveTimer_ = 0.0f;
	float tackleMoveTime_ = 0.0f;

	float tackleSpeed_ = 0.0f;

	// ===== 3連続タックル用 =====
	int tackleCount_ = 0; // 現在何回目か
	const int maxTackleCount_ = 3;
};