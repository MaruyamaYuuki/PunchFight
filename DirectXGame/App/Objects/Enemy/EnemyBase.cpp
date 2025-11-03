#include "EnemyBase.h"
#include <cassert>

using namespace KamataEngine;

void EnemyBase::Initialize(const EnemyData& data) {
	model_ = Model::CreateFromOBJ(data.modelPath, true);
	speed_ = data.speed;
	hp_ = data.hp;
	attackPower_ = data.attackPower;
	worldTransform_.Initialize();
}

void EnemyBase::Update(const KamataEngine::Vector3& playerPos) { 
	// プレイヤーをの距離を測る
	float dx = playerPos.x - worldTransform_.translation_.x;
	float dz = playerPos.z - worldTransform_.translation_.z;
	distanceToPlayer_ = sqrtf(dx * dx + dz * dz);

	worldTransform_.UpdateMatrix(); 
}

void EnemyBase::Draw(Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}

void EnemyBase::SetPosition(float x, float y, float z) {
	worldTransform_.translation_ = {x, y, z}; 
}
