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

	void NormalAttack(const KamataEngine::Vector3& playerPos);

	void TackleAttack(const KamataEngine::Vector3& playerPos);

protected:
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

	void UpdateTextures() override;

private:
	uint32_t RTackleTexture_ = 0;
	uint32_t LTackleTexture_ = 0;

	bool useTackle_ = false;

	AttackType attackType_ = AttackType::Normal;
};
