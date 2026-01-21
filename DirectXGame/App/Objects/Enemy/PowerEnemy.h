#pragma once
#include "EnemyBase.h"

/// <summary>
/// 攻撃の種類
/// </summary>
enum class AttackType {
	Normal,
	Tackle
};

/// <summary>
/// パワータイプの敵
/// </summary>
class PowerEnemy : public EnemyBase {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	PowerEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data">エネミーデータ</param>
	void Initialize(const EnemyData& data) override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) override;

private:

	void TackleAttack();

	void EnterAttackMode(const KamataEngine::Vector3& playerPos);

protected:
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

	void UpdateTextures() override;



private:
	MyEngine::GameConfigManager* cfg_ = nullptr;

	uint32_t RTackleTexture_ = 0;
	uint32_t RTackleWaitTexture_ = 0;
	uint32_t LTackleTexture_ = 0;
	uint32_t LTackleWaitTexture_ = 0;

	bool useTackle_ = true;

	AttackType attackType_ = AttackType::Normal;

	// タックル用
	bool isTackleCharging_ = false;
	bool isTackling_ = false;
	float tackleProbability_; // %の確率でタックル攻撃

	float tackleChargeTimer_ = 0.0f;
	float tackleChargeTime_; // 溜め時間

	float tackleMoveTimer_ = 0.0f;
	float tackleMoveTime_; // 突進時間

	float tackleSpeed_;
};
