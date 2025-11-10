#include "EnemyBase.h"

using namespace KamataEngine;

void EnemyBase::Initialize(const EnemyData& data) {
	model_ = Model::CreateFromOBJ(data.modelPath, true);
	speed_ = data.speed;
	hp_ = data.hp;
	attackPower_ = data.attackPower;

	worldTransform_.Initialize();
}

void EnemyBase::Update(const Vector3&) {
	// ワールド変換更新
	worldTransform_.UpdateMatrix();

	// ヒットボックス座標を敵のワールド座標に合わせる
	hitBox_.pos = worldTransform_.translation_;
}

void EnemyBase::Draw(Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}

void EnemyBase::OnHit(int damage) {
	hp_ -= damage;
	if (hp_ < 0)
		hp_ = 0;
}

void EnemyBase::SetPosition(float x, float y, float z) { worldTransform_.translation_ = {x, y, z}; }

void EnemyBase::SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

void EnemyBase::SetHitBox(const Vector3& center, const Vector3& size) {
	hitBox_.pos = center;
	hitBox_.size = size;
	hitBox_.active = true;
}
