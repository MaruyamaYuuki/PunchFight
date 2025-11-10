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
	if (isKnockback_) {

		knockbackTime_ += deltaTime;

		// 減速用の係数
		float drag = 0.95f;

		// X方向に加速して減速していく
		knockbackVelocity_.x *= drag;
		worldTransform_.translation_.x += knockbackVelocity_.x * deltaTime;

		// Y方向は重力で落下させる
		knockbackVelocity_.y -= gravity_ * deltaTime;
		worldTransform_.translation_.y += knockbackVelocity_.y * deltaTime;

		// 規定時間で消滅
		if (knockbackTime_ >= knockbackDuration_) {
			isKnockback_ = false;
			isDead_ = true;
		}
	}

	worldTransform_.UpdateMatrix();
	hitBox_.pos = worldTransform_.translation_;
}




void EnemyBase::Draw(Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera);
	}
}

void EnemyBase::OnHit(int damage, const Vector3& attackDir) {
	hp_ -= damage;

	if (hp_ <= 0 && !isKnockback_) {
		hp_ = 0;
		isKnockback_ = true;
		knockbackTime_ = 0.0f;

		// スマブラ風 初速
		float basePower = 25.0f;   // 吹っ飛び強さ
		float upwardBoost = 12.0f; // 上方向の初速

		knockbackVelocity_ = {attackDir.x * basePower, upwardBoost, 0.0f};
	}
}


void EnemyBase::SetPosition(float x, float y, float z) { worldTransform_.translation_ = {x, y, z}; }

void EnemyBase::SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

void EnemyBase::SetHitBox(const Vector3& center, const Vector3& size) {
	hitBox_.pos = center;
	hitBox_.size = size;
	hitBox_.active = true;
}
