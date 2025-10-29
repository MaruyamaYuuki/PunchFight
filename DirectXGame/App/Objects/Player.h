#pragma once

#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public:
	~Player() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(KamataEngine::Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(KamataEngine::Camera& camera);

	void Reset();

	const WorldTransformEx& GetWorldTransform() const { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return move; }

	void UpdateWorldTransform() { worldTransform_.UpdateMatrix(); }

	bool IsDead() const { return isDead_; }

	void TakeDamage(int damage);

	int GetHP() const { return HP_; }

private:
	void Move();

	void Attack();

	void AttackUpdate();

	void TextureUpdate();

private:
	KamataEngine::Input* input_ = nullptr;

	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelDebugHitBox_ = nullptr;

	bool isDead_ = false;
	int HP_ = 100;

	// --- プレイヤーのテクスチャ ---
	uint32_t textureHandle_ = 0;
	uint32_t RPlayerTexture_ = 0;
	uint32_t RLeftPunchTexture_ = 0;
	uint32_t RRightPunchTexture_ = 0;
	uint32_t RUppercutTexture_ = 0;
	uint32_t RKnockDownTexture_ = 0;
	uint32_t RKnockDown2Texture_ = 0;

	uint32_t LPlayerTexture_ = 0;
	uint32_t LLeftPunchTexture_ = 0;
	uint32_t LRightPunchTexture_ = 0;
	uint32_t LUppercutTexture_ = 0;
	uint32_t LKnockDownTexture_ = 0;
	uint32_t LKnockDown2Texture_ = 0;

	// --- 移動関連 ---
	KamataEngine::Vector3 move = {0, 0, 0};
	float moveSpeed = 0.1f; // 通常移動速度

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
	struct HitBox {
		KamataEngine::Vector3 pos;  // ワールド座標
		KamataEngine::Vector3 size; // 当たり範囲
		bool active;  // 現在有効か
	};
	HitBox hitBox_; // 現在のパンチのヒットボックス

	// --- ノックダウン処理用 ---
	float knockDownTimer_ = 2.0f;
	float deltaTime = 1.0f / 60.0f;
};
