#pragma once
#include "KamataEngine.h"
#include "../../../Engine/Math/WorldTransformEx.h"

struct EnemyData {
	std::string modelPath;
	std::string texturePath;
	float speed;
	int hp;
	float attackPower;
};

class EnemyBase {
public:
    virtual ~EnemyBase() = default;

    virtual void Initialize(const EnemyData& data);

	virtual void Update(const KamataEngine::Vector3 playerPos);

	virtual void Draw(KamataEngine::Camera& camera);

	void SetPosition(float x, float y, float z);
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

protected:
	virtual void Attack();

protected:
	WorldTransformEx worldTransform_;
	KamataEngine::Model* model_ = nullptr;

	uint32_t textureHandle_ = 0;

	float speed_ = 0.0f;
	int hp_ = 1;
	float attackPower_ = 1.0f;

	float distanceToPlayer_ = 0.0f;
	float AttackTimer_ = 0.0f;
	bool isAttacking_ = false;
};
