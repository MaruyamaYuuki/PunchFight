#include "EnemyBase.h"

using namespace KamataEngine;

void EnemyBase::Initialize(const EnemyData& data) {
	model_ = Model::CreateFromOBJ(data.modelPath, true);
	speed_ = data.speed;
	hp_ = data.hp;
	attackPower_ = data.attackPower;
	worldTransform_.Initialize();
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
