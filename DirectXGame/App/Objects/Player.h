#pragma once

#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine//Math/Collider.h"
#include "../../Engine/Particle/SmokeParticleManager.h"

/// <summary>
/// プレイヤー
/// </summary>
class GameConfigManager;
class Player {
public:
	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~Player() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelSP, KamataEngine::Model* modelBox, KamataEngine::Vector3 pos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

	/// <summary>
	/// プレイヤーの状態のリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// WorldTransformだけを更新
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// 接触判定
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	void OnHit(int32_t damage);

	/// <summary>
	/// プレイヤーのWorldTransformを設定する
	/// </summary>
	/// <returns>プレイヤーのWorldTransform</returns>
	const WorldTransformEx& GetWorldTransform() const { return worldTransform_; }

	/// <summary>
	/// プレイヤーの移動量を取得する
	/// </summary>
	/// <returns>プレイヤーの移動量</returns>
	const KamataEngine::Vector3& GetVelocity() const { return move_; }

	/// <summary>
	/// プレイヤーの死亡判定
	/// </summary>
	/// <returns>死亡していれば true、生存していれば false</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// プレイヤーのHPを取得する
	/// </summary>
	/// <returns></returns>
	int32_t GetHP() const { return HP_; }

	/// <summary>
	/// プレイヤーのヒットボックスを取得する
	/// </summary>
	/// <returns>当たり判定の情報（位置・サイズ・有効状態など）</returns>
	HitBox GetPlayerHitBox() const { return playerHitBox_; }

	/// <summary>
	/// 攻撃のヒットボックスを取得する
	/// </summary>
	/// <returns>攻撃判定の情報（位置・サイズ・有効状態など）</returns>
	HitBox GetAttackHitBox() const { return nAttackHitBox_; }

	/// <summary>
	/// 強攻撃のヒットボックスを取得する
	/// </summary>
	/// <returns>強攻撃判定の情報（位置・サイズ・有効状態など）</returns>
	HitBox GetSPAttackHitBox() const { return spAttackHitBox_; }

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>プレイヤーの攻撃力</returns>
	int32_t GetAttackPower() const { return nAttackPower_; }

	/// <summary>
	/// 強攻撃の攻撃力を取得する
	/// </summary>
	/// <returns>強攻撃の攻撃力</returns>
	int32_t GetSPAttackPower() const { return spAttackPower_; }

	/// <summary>
	/// プレイヤーの向いてる方向を取得する
	/// </summary>
	/// <returns>右向きなら 1.0f、左向きなら -1.0f を返す</returns>
	float GetFacingDir() const { return facingDir_; }

	/// <summary>
	/// 強攻撃の発射方向を取得
	/// </summary>
	/// <returns>右方向なら 1.0f、左方向なら -1.0f を返す</returns>
	float GetSPAttackDir() const { return spAttackDirection_; }

	/// <summary>
	/// 目標座標に着いたか
	/// </summary>
	/// <returns>目標座標に着いているなら true、着いてないなら false</returns>
	bool IsGoal() { return isGoal_; }

	/// <summary>
	/// ポーズをとったか
	/// </summary>
	/// <returns>ポーズをとっているなら true、とってないなら false</returns>
	bool IsVictory() { return isVictory_; }

	/// <summary>
	/// ステップ中かを取得
	/// </summary>
	/// <returns>ステップ中なら true、してなければ false</returns>
	bool CanStep() const { return !isStepping_ && stepTimer_ <= 0; }

	/// <summary>
	/// ステップのクールタイムを取得
	/// </summary>
	/// <returns>ステップのクールタイム</returns>
	int32_t GetStepCooldownTimer() const { return stepTimer_; }

	/// <summary>
	/// ステップのクールタイムの最大値を取得
	/// </summary>
	/// <returns>ステップのクールタイムの最大値</returns>
	int32_t GetStepCooldownMax() const { return stepCooldown_; }

	/// <summary>
	/// 強攻撃を使ったかを取得
	/// </summary>
	/// <returns>使っていれば true、いなければ false</returns>
	bool CanSpecialAttack() const { return canSpecialAttack_; }

	/// <summary>
	/// 強攻撃のクールタイムを取得
	/// </summary>
	/// <returns>強攻撃のクールタイム</returns>
	float GetSPAttackCooldownTimer() const { return spAttackCooldownTimer_; }

	/// <summary>
	/// 強攻撃のクールタイムの最大値を取得
	/// </summary>
	/// <returns>強攻撃のクールタイムの最大値</returns>
	float GetSPAttackCooldownMax() const { return spAttackCoolDown_; }

	bool DidSpecialAttack() const { return justSpecialAttacked_; }

	/// <summary>
	/// X軸での移動限界座標を設定する
	/// </summary>
	/// <param name="limit">移動限界</param>
	void SetEndMoveLimitX(float limit) { endMoveLimitX_ = limit; }

	/// <summary>
	/// X軸での回転角を設定する
	/// </summary>
	/// <param name="rotX">回転角</param>
	void SetRotateX(float rotX) { worldTransform_.rotation_.x = rotX; }

	/// <summary>
	/// クリアシーンでのアニメーション
	/// </summary>
	/// <param name="isSpot">スポットライト</param>
	void ClearAnimation();

private:
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 攻撃の更新
	/// </summary>
	void AttackUpdate();

	/// <summary>
	/// 強攻撃
	/// </summary>
	void SpecialAttack();

	/// <summary>
	/// 強攻撃の更新
	/// </summary>
	void SpecialAttackUpdate();

	/// <summary>
	/// テクスチャの更新
	/// </summary>
	void TextureUpdate();

private:
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	GameConfigManager* cfg_ = nullptr;

	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformSP_;
	WorldTransformEx worldTransformNormalAttackHitBox_;
	WorldTransformEx worldTransformSPHitBox_;
	WorldTransformEx worldTransformPHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelSpecial_ = nullptr;
	KamataEngine::Model* modelDebugHitBox_ = nullptr;
	KamataEngine::Model* modelHitBox_ = nullptr;
	KamataEngine::Model* modelSPHiyBox_ = nullptr;

	// --- プレイヤーのテクスチャ ---
	uint32_t textureHandle_ = 0;
	uint32_t RPlayerTexture_ = 0;
	uint32_t RLeftPunchTexture_ = 0;
	uint32_t RRightPunchTexture_ = 0;
	uint32_t RUppercutTexture_ = 0;
	uint32_t RKnockDownTexture_[2];
	uint32_t RRunTexture_[3];

	uint32_t LPlayerTexture_ = 0;
	uint32_t LLeftPunchTexture_ = 0;
	uint32_t LRightPunchTexture_ = 0;
	uint32_t LUppercutTexture_ = 0;
	uint32_t LKnockDownTexture_[2];
	uint32_t LRunTexture_[3];

	// --- 強攻撃のテクスチャ ---
	uint32_t SPTextureHandle_ = 0;
	uint32_t RSpecialTexture_ = 0;
	uint32_t LSpecialTexture_ = 0;

	uint32_t smokeTexture_ = 0;

	// --- SE ---
	uint32_t normalAttackSEDataHandle_ = 0;
	uint32_t spAttackSEDataHandle_ = 0;
	uint32_t hitSEDataHandle_ = 0;

	uint32_t normalAttackSEVoiceHandle_ = 0;
	uint32_t spAttackSEVoiceHandle_ = 0;
	uint32_t hitSEVoiceHandle_ = 0;

	bool isDead_ = false;
	int32_t HP_;

	// --- 移動関連 ---
	KamataEngine::Vector3 move_ = {0, 0, 0};
	float moveSpeed_; // 通常移動速度
	int32_t walkFrame_ = 0;         // 0〜3でループ
	int32_t walkFrameTimer_ = 0;    // テクスチャ切替タイマー
	int32_t walkFrameInterval_; // 何フレームごとに切り替えるか

	// --- ステップ関連 ---
    bool isStepping_ = false;
	KamataEngine::Vector3 stepDirection_ = {0, 0, 0};
	int32_t stepTimer_ = 0;
	int32_t stepCooldown_; // 例：1秒クールタイム（60FPS想定）
	int32_t stepFrame_ = 0;
	float stepPower_ ; // 通常移動の3倍速

    // --- 攻撃管理 ---
	bool isNormalAttacking_ = false;    // 攻撃中か
	bool canNormalAttack_ = true;       // 攻撃可能か
	int32_t nAttackTimer_ = 0;         // 攻撃残り時間
	int32_t nAttackCooldownTimer_ = 0; // クールタイム残り時間
	int32_t nAttackPower_;

	int32_t nAttackDuration_; // 攻撃の長さ
	int32_t nAttackCooldown_; // クールタイムの長さ

	bool nAttackFromRight_ = true; // 右 or 左パンチ切り替え
	float facingDir_ = 1.0f;      // 向き（1.0f：右, -1.0f：左）

	// --- 強攻撃管理 ---
	bool isSpecialAttacking_ = false;
	bool canSpecialAttack_ = false;
	float spAttackTimer_ = 0.0f;
	float spAttackCooldownTimer_ = 0.0f;
	float spAttackMoveSpeed_;
	int32_t spAttackPower_;

	float spAttackDuration_;
	float spAttackCoolDown_;
	const float deltaTime_ = 1.0f / 60.0f;

	float spAttackDirection_ = 1.0f; // 気弾の発射方向を記録
	bool justSpecialAttacked_ = false;

	// --- ヒットボックス ---
	HitBox playerHitBox_;
	HitBox nAttackHitBox_; // 現在のパンチのヒットボックス
	HitBox spAttackHitBox_;

	// --- ノックダウン処理用 ---
	float knockDownTimer_;

	float endMoveLimitX_ = 0;

	// ---クリア演出用 ---
	bool isGoal_ = false;
	bool isVictory_ = false;
	float poseWaitTimer_;

	// --- パーティクル用 ---
	std::unique_ptr<SmokeParticleManager> smokeManager_;
	float trailSpawnTimer_ = 0.0f;
	float trailSpawnInterval_;
	KamataEngine::Vector3 smokeSize_;

};
