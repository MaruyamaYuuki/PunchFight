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
	EnemyBase() = default; // デフォルトコンストラクタ追加
	virtual ~EnemyBase() = default;

	virtual void Initialize(const EnemyData& data);
	virtual void Update(const KamataEngine::Vector3&) {}
	virtual void Draw(KamataEngine::Camera& camera);
	virtual void OnHit(int damage);

	bool IsDead() const { return hp_ <= 0; }
	const HitBox& GetHitBox() const { return attackBox_; }
	void SetPosition(float x, float y, float z);
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

protected:
	WorldTransformEx worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	float speed_ = 0.0f;
	int hp_ = 1;
	float attackPower_ = 1.0f;

	HitBox attackBox_;
	float deltTime_ = 1.0f / 60.0f;
};
