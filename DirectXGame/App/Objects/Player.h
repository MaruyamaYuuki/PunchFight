#pragma once

#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

class Player {
public:

	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw(KamataEngine::Camera& camera);

	void Move();

	void Attack();

	void AttackUpdate();

	const WorldTransformEx& GetWorldTransform() const { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return move; }

private:
	KamataEngine::Input* input_ = nullptr;

	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelDebugHitBox_ = nullptr;

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
	const int attackCooldown_ = 20; // クールタイムの長さ

	bool attackFromRight_ = true; // 右 or 左パンチ切り替え
	float facingDir_ = 1.0f;      // 向き（1.0f：右, -1.0f：左）

	// --- ヒットボックス ---
	struct HitBox {
		KamataEngine::Vector3 pos;  // ワールド座標
		KamataEngine::Vector3 size; // 当たり範囲
		bool active;  // 現在有効か
	};
	HitBox hitBox_; // 現在のパンチのヒットボックス
};
