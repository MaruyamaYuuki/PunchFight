#pragma once

#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine//Math/Collider.h"

/// <summary>
/// プレイヤー
/// </summary>
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
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelBox, KamataEngine::Vector3& pos);

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
	/// プレイヤーのWorldTransformを設定する
	/// </summary>
	/// <returns>プレイヤーのWorldTransform</returns>
	const WorldTransformEx& GetWorldTransform() const { return worldTransform_; }

	/// <summary>
	/// プレイヤーの移動量を取得する
	/// </summary>
	/// <returns>プレイヤーの移動量</returns>
	const KamataEngine::Vector3& GetVelocity() const { return move; }

	/// <summary>
	/// WorldTransformだけを更新
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// プレイヤーの死亡判定
	/// </summary>
	/// <returns>死亡していれば true、生存していれば false</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 受けるダメージ量を設定する
	/// </summary>
	/// <param name="damage"></param>
	void TakeDamage(int damage);

	/// <summary>
	/// プレイヤーのHPを取得する
	/// </summary>
	/// <returns></returns>
	int GetHP() const { return HP_; }

	/// <summary>
	/// 攻撃のヒットボックスを取得する
	/// </summary>
	/// <returns>攻撃判定の情報（位置・サイズ・有効状態など）</returns>
	HitBox GetAttackHitBox() const { return attackHitBox_; }

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>プレイヤーの攻撃力</returns>
	int GetAttackPower() const { return attackPower_; }

	/// <summary>
	/// プレイヤーの向いてる方向を取得する
	/// </summary>
	/// <returns>右向きなら 1.0f、左向きなら -1.0f を返す</returns>
	float GetFacingDir() const { return facingDir_; }

	/// <summary>
	/// X軸での移動限界座標を設定する
	/// </summary>
	/// <param name="limit">移動限界</param>
	void SetEndMoveLimitX(float limit) { endMoveLimitX = limit; }

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
	/// テクスチャの更新
	/// </summary>
	void TextureUpdate();

private:
	KamataEngine::Input* input_ = nullptr;

	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformHitBox_;
	WorldTransformEx worldTransformPHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelDebugHitBox_ = nullptr;
	KamataEngine::Model* modelHitBox_ = nullptr;

	bool isDead_ = false;
	int HP_ = 100;
	int attackPower_ = 10;

	// --- プレイヤーのテクスチャ ---
	uint32_t textureHandle_ = 0;
	uint32_t RPlayerTexture_ = 0;
	uint32_t RLeftPunchTexture_ = 0;
	uint32_t RRightPunchTexture_ = 0;
	uint32_t RUppercutTexture_ = 0;
	uint32_t RKnockDownTexture_ = 0;
	uint32_t RKnockDown2Texture_ = 0;
	uint32_t RRunTexture1_ = 0;
	uint32_t RRunTexture2_ = 0;
	uint32_t RRunTexture3_ = 0;

	uint32_t LPlayerTexture_ = 0;
	uint32_t LLeftPunchTexture_ = 0;
	uint32_t LRightPunchTexture_ = 0;
	uint32_t LUppercutTexture_ = 0;
	uint32_t LKnockDownTexture_ = 0;
	uint32_t LKnockDown2Texture_ = 0;
	uint32_t LRunTexture1_ = 0;
	uint32_t LRunTexture2_ = 0;
	uint32_t LRunTexture3_ = 0;

	// --- 移動関連 ---
	KamataEngine::Vector3 move = {0, 0, 0};
	float moveSpeed = 0.05f; // 通常移動速度
	int walkFrame_ = 0;         // 0〜3でループ
	int walkFrameTimer_ = 0;    // テクスチャ切替タイマー
	int walkFrameInterval_ = 6; // 何フレームごとに切り替えるか

	// --- ステップ関連 ---
    bool isStepping_ = false;
	KamataEngine::Vector3 stepDirection_ = {0, 0, 0};
	int stepTimer_ = 0;
	int stepCooldown_ = 30; // 例：1秒クールタイム（60FPS想定）
	int stepFrame_ = 0;
	float stepPower_ = 2.5f; // 通常移動の3倍速

    // --- 攻撃管理 ---
	bool isAttacking_ = false;    // 攻撃中か
	bool canAttack_ = true;       // 攻撃可能か
	int attackTimer_ = 0;         // 攻撃残り時間
	int attackCooldownTimer_ = 0; // クールタイム残り時間

	const int attackDuration_ = 15; // 攻撃の長さ
	const int attackCooldown_ = 5; // クールタイムの長さ

	bool attackFromRight_ = true; // 右 or 左パンチ切り替え
	float facingDir_ = 1.0f;      // 向き（1.0f：右, -1.0f：左）

	// --- ヒットボックス ---
	HitBox playerHitBox_;
	HitBox attackHitBox_; // 現在のパンチのヒットボックス

	// --- ノックダウン処理用 ---
	float knockDownTimer_ = 2.0f;
	float deltaTime = 1.0f / 60.0f;

	float endMoveLimitX = 0;
};
