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

	bool IsKnockBack() const { return isKnockback_; }

protected:
	WorldTransformEx worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	float speed_ = 0.0f;
	int hp_ = 1;
	float attackPower_ = 1.0f;

	HitBox hitBox_;

	float deltaTime = 1.0f / 60.0f;

    bool isDead_ = false;      // 完全に消えたか
    bool isKnockback_ = false;
	float knockbackTime_ = 0.0f;
	float knockbackSpeed_ = 5.0f;    // X方向の吹っ飛び速度
	float knockbackDuration_ = 0.5f; // 0.5秒間吹っ飛ぶ
	float knockbackHeight_ = 2.0f;   // Y方向の少し浮く高さ
	KamataEngine::Vector3 knockbackDir_;
	KamataEngine::Vector3 knockbackVelocity_{0, 0, 0};
	float gravity_ = 30.0f;
};
