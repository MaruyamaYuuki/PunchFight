// BossEnemy.h
#pragma once
#include "EnemyBase.h"

/// <summary>
/// 攻撃の種類
/// </summary>
enum class BossAttackType { 
	Normal, 
	TripleTackle, 
	Jump };

/// <summary>
/// ボス専用クラス
/// ・通常攻撃
/// ・3連続タックル攻撃
/// </summary>
class BossEnemy : public EnemyBase {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data">エネミーデータ</param>
	void Initialize(const EnemyData& data) override;

	/// 更新処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（追従対象）</param>
	/// <param name="allEnemies">全エネミーのリスト（他の敵との重なり回避計算に使用）</param>
	void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera) override;

	// 攻撃タイプに応じて返すテクスチャを切り替える
	uint32_t GetRWaitTexture() const override;
	uint32_t GetRAttackTexture() const override;
	uint32_t GetLWaitTexture() const override;
	uint32_t GetLAttackTexture() const override;

private:

	/// <summary>
	/// BossEnemy固有の攻撃処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（攻撃ベクトル計算などに使用）</param>
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

	/// <summary>
	/// 三回連続タックル攻撃を行う処理
	/// </summary>
	void TripleTackleAttack();

	/// <summary>
	/// 攻撃開始時の初期化
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（攻撃ベクトル計算などに使用）</param>
	void EnterAttackMode(const KamataEngine::Vector3& playerPos);

	/// <summary>
	/// ジャンプ攻撃を行う処理
	/// </summary>
	void JumpAttack();

	/// <summary>
	/// 衝撃波攻撃を行う処理
	/// </summary>
	void ShockWaveAttack();

	// 衝撃波攻撃の状態を返す
	bool IsShockWaveActive() const override { return shockWaveActive_; }

	// 当たり判定の半径として「現在の半径」を返すようにオーバーライド
	float GetShockWaveRadius() const override { return currentShockWaveRadius_; }

private:
	uint32_t RTackleTexture_ = 0;
	uint32_t RTackleWaitTexture_ = 0;
	uint32_t RJumpAttackTexture_ = 0;
	uint32_t LTackleTexture_ = 0;
	uint32_t LTackleWaitTexture_ = 0;
	uint32_t LJumpAttackTexture_ = 0;

	std::unique_ptr<KamataEngine::Model> model_;

	BossAttackType attackType_ = BossAttackType::Normal;

	// ===== 衝撃波可視化用 =====
	std::unique_ptr<KamataEngine::Model> shockWaveModel_;
	MyEngine::WorldTransformEx shockWaveTransform_;

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

	// ==== ジャンプ攻撃 ====
	bool useJumpAttack_ = false;

	bool isJumpCharging_ = false;
	bool isJumping_ = false;
	bool isLanding_ = false;

	float jumpChargeTimer_ = 0.0f;
	float jumpChargeTime_ = 0.5f;

	float jumpVelocityY_ = 0.0f;
	float jumpPower_ = 10.0f;
	float jumpGravity_ = 20.0f;

	float groundY_ = 0.0f;
	bool shockWaveActive_ = false;
	float shockWaveTime_ = 0.5f;
	float shockWaveTimer_ = 0.0f;
	// 衝撃波の広がりを管理する変数
	float currentShockWaveRadius_ = 0.0f; // 現在の半径
	float maxShockWaveRadius_ = 3.0f;     // 最大まで広がった時の半径
};