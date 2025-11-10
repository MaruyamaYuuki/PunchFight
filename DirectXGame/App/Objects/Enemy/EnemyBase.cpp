#include "EnemyBase.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void EnemyBase::Initialize(const EnemyData& data) {
	model_ = Model::CreateFromOBJ(data.modelPath, true);
	speed_ = data.speed;
	hp_ = data.hp;
	attackPower_ = data.attackPower;

	modelEHitBox_ = Model::CreateFromOBJ("boxFrame", true);
	modelAHitBox_ = Model::CreateFromOBJ("boxFrame", true);

	worldTransform_.Initialize();
	worldTransformEHitBox_.Initialize();
	worldTransformAHitBox_.Initialize();
}

void EnemyBase::Update(const Vector3&) {
	if (isKnockBack_) {

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
			isKnockBack_ = false;
			isDead_ = true;
		}
	}

	// ===== スタン処理 =====
	if (isStan_) {
		stanTimer_ -= deltaTime;

		if (stanTimer_ <= 0.0f) {
			isStan_ = false;
		}
	}

	worldTransform_.UpdateMatrix();
	worldTransformEHitBox_.UpdateMatrix();
	worldTransformAHitBox_.UpdateMatrix();
	hitBox_.pos = worldTransform_.translation_;

}




void EnemyBase::Draw(Camera& camera) {
	if (model_) {
		model_->Draw(worldTransform_, camera, textureHandle_);
	}
	if (hitBox_.active) {
		worldTransformEHitBox_.translation_ = hitBox_.pos;
		worldTransformEHitBox_.scale_ = hitBox_.size;
		modelEHitBox_->Draw(worldTransformEHitBox_, camera);
	}
	if (attackHitBox_.active) {
		worldTransformAHitBox_.translation_ = attackHitBox_.pos;
		worldTransformAHitBox_.scale_ = attackHitBox_.size;
		modelAHitBox_->Draw(worldTransformAHitBox_, camera);
	}
}

void EnemyBase::OnHit(int damage, const Vector3& attackDir) {
	hp_ -= damage;
	isStan_ = true;
	stanTimer_ = stanDuration_;

	if (hp_ <= 0 && !isKnockBack_) {
		hp_ = 0;
		isKnockBack_ = true;
		knockbackTime_ = 0.0f;

		// スマブラ風 初速
		float basePower = 25.0f;   // 吹っ飛び強さ
		float upwardBoost = 12.0f; // 上方向の初速

		knockbackVelocity_ = {attackDir.x * basePower, upwardBoost, 0.0f};
	}
}


void EnemyBase::SetPosition(float x, float y, float z) { worldTransform_.translation_ = {x, y, z}; }

void EnemyBase::SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

void EnemyBase::SetAttackHitBox(const Vector3& pos) {
	attackHitBox_.pos = pos;
	attackHitBox_.size = {0.2f, 0.5f, 0.5f};
	attackHitBox_.active = true;
}

void EnemyBase::UpdateTextures() {
	switch (state_) {
	case EnemyState::Idle:
		if (facingDir_ == 1.0f) {
    		textureHandle_ = RIdleTexture_;
		} else {
			textureHandle_ = LIdleTexture_;
		}
		break;
	case EnemyState::Walking:
		if (facingDir_ == 1.0f) {
			textureHandle_ = RIdleTexture_;
		} else {
			textureHandle_ = LIdleTexture_;
		}
		break;
	case EnemyState::Attacking:
		if (facingDir_ == 1.0f) {
			textureHandle_ = RAttackTexture_;
		} else {
			textureHandle_ = LAttackTexture_;
		}
		break;
	case EnemyState::Stunned:
		if (facingDir_ == 1.0f) {
			textureHandle_ = RIdleTexture_;
		} else {
			textureHandle_ = LIdleTexture_;
		}
		break;
	case EnemyState::Knockback:
		if (facingDir_ == 1.0f) {
			textureHandle_ = RIdleTexture_;
		} else {
			textureHandle_ = LIdleTexture_;
		}
		break;
	case EnemyState::Dead:
		if (facingDir_ == 1.0f) {
			textureHandle_ = RIdleTexture_;
		} else {
			textureHandle_ = LIdleTexture_;
		}
		break;
	}
}

void EnemyBase::SetHitBox(const Vector3& center, const Vector3& size) {
	hitBox_.pos = center;
	hitBox_.size = size;
	hitBox_.active = true;
}
