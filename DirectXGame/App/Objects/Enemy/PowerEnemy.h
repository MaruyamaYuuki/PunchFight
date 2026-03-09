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
/// 高い攻撃力や耐久力を持つ、重量級エネミーの挙動実装。
/// プレイヤーの攻撃を受けても怯みにくい（スーパーアーマー等）特殊な被弾判定の制御。
// 広範囲、あるいは高威力の固有攻撃パターンのAIロジック実行。
/// NormalEnemy とは異なる移動アルゴリズム（重い足取り、突進など）の制御。
///強敵としての視覚演出（大きなノックバックやエフェクト）のトリガー管理。
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

	/// 更新処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（追従対象）</param>
	/// <param name="allEnemies">全エネミーのリスト（他の敵との重なり回避計算に使用）</param>
	virtual void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies);

private:
	/// <summary>
	/// 突進攻撃（タックル）の物理挙動およびタイマー制御
	/// </summary>
	void TackleAttack();

	/// <summary>
	/// 攻撃モードへの遷移判定と初期化
	/// </summary>
	/// <param name="playerPos">ターゲットとなるプレイヤーの座標</param>
	void EnterAttackMode(const KamataEngine::Vector3& playerPos);

protected:
	/// <summary>
	/// 通常攻撃とタックル攻撃の分岐・実行制御
	/// </summary>
	/// <param name="playerPos">プレイヤーの現在座標</param>
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

	/// <summary>
	/// 重量級エネミー専用のテクスチャ（タックル予備動作等）への更新
	/// </summary>
	void UpdateTextures() override;

	/// <summary>
	/// 動きを止めるべき状態かどうかを判定する
	/// </summary>
	/// <returns>ノックバック中、スタン中に0を返す</returns>
	bool IsMovementInterrupted() const override {
		// PowerEnemyはスタン中(isStun_)でも false を返すことで、Updateを続行させる
		return IsKnockBack() || GetHP() <= 0;
	}

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
