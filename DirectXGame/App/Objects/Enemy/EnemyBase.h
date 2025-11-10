#pragma once
#include "../../../Engine/Math/Collider.h"
#include "../../../Engine/Math/WorldTransformEx.h"
#include "KamataEngine.h"

struct EnemyData {
	std::string modelPath;
	float speed;
	int hp;
	float attackPower;
};

enum class EnemyState { 
	Idle, 
	Walking, 
	Attacking, 
	Stunned, 
	Knockback, 
	Dead 
};

class EnemyBase {
public:
	EnemyBase() = default;
	virtual ~EnemyBase() = default;

	virtual void Initialize(const EnemyData& data);
	virtual void Update(const KamataEngine::Vector3& playerPos);
	virtual void Draw(KamataEngine::Camera& camera);
	virtual void OnHit(int damage, const KamataEngine::Vector3& attackDir);

	bool IsDead() const { return isDead_; }

	const HitBox& GetHitBox() const { return hitBox_; }
	void SetHitBox(const KamataEngine::Vector3& center, const KamataEngine::Vector3& size);

	void SetPosition(float x, float y, float z);
	void SetScale(const KamataEngine::Vector3& scale);
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	int GetHP() const { return hp_; }

	bool IsKnockBack() const { return isKnockBack_; }

	void SetAttackHitBox(const KamataEngine::Vector3& pos);

	void UpdateTextures();

protected:
	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformEHitBox_;
	WorldTransformEx worldTransformAHitBox_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelEHitBox_;
	KamataEngine::Model* modelAHitBox_;
	uint32_t textureHandle_ = 0;

	float speed_ = 0.0f;
	int hp_ = 1;
	float attackPower_ = 1.0f;
	float facingDir_ = 1.0f; // 向き（1.0f：右, -1.0f：左）
	int attackCooldownTimer_ = 0; // クールタイム残り時間
	const int attackDuration_ = 15; // 攻撃の長さ
	const int attackCooldown_ = 30;  // クールタイムの長さ
	/// ---ひるみ---
	bool isStan_ = false;
	float stanTimer_ = 0.0f;
	float stanDuration_ = 0.2f; // ひるみ時間

	HitBox hitBox_;
	HitBox attackHitBox_;

	float deltaTime = 1.0f / 60.0f;

    bool isDead_ = false;      // 完全に消えたか
    bool isKnockBack_ = false;
	float knockbackTime_ = 0.0f;
	float knockbackSpeed_ = 5.0f;    // X方向の吹っ飛び速度
	float knockbackDuration_ = 0.5f; // 0.5秒間吹っ飛ぶ
	float knockbackHeight_ = 2.0f;   // Y方向の少し浮く高さ
	KamataEngine::Vector3 knockbackDir_;
	KamataEngine::Vector3 knockbackVelocity_{0, 0, 0};
	float gravity_ = 30.0f;

	EnemyState state_ = EnemyState::Idle;
	uint32_t RIdleTexture_ = 0;
	uint32_t RAttackTexture_ = 0;
	uint32_t LIdleTexture_ = 0;
	uint32_t LAttackTexture_ = 0;
};
