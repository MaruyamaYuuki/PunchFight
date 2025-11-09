#define NOMINMAX
#include "Player.h"
#include <cassert>
#include <algorithm>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void Player::Initialize(Model* model, KamataEngine::Model* modelBox) { 
	input_ = Input::GetInstance(); 

	assert(model);
	model_ = model;
	assert(modelBox);
	modelDebugHitBox_ = modelBox;

	worldTransform_.Initialize();
	worldTransform_.translation_.y += 1.0f;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformHitBox_.Initialize();

	textureHandle_ = TextureManager::Load("playerTextures/RPlayer.png");

	// 右向きテクスチャ
	RPlayerTexture_ = TextureManager::Load("playerTextures/RPlayer.png");
	RLeftPunchTexture_ = TextureManager::Load("playerTextures/RLeftPunch.png");
	RRightPunchTexture_ = TextureManager::Load("playerTextures/RRightPunch.png");
	RUppercutTexture_ = TextureManager::Load("playerTextures/RUppercut.png");
	RKnockDownTexture_ = TextureManager::Load("playerTextures/RKnockDown.png");
	RKnockDown2Texture_ = TextureManager::Load("playerTextures/RKnockDown2.png");
	// 左向きテクスチャ
	LPlayerTexture_ = TextureManager::Load("playerTextures/LPlayer.png");
	LLeftPunchTexture_ = TextureManager::Load("playerTextures/LLeftPunch.png");
	LRightPunchTexture_ = TextureManager::Load("playerTextures/LRightPunch.png");
	LUppercutTexture_ = TextureManager::Load("playerTextures/LUppercut.png");
	LKnockDownTexture_ = TextureManager::Load("playerTextures/LKnockDown.png");
	LKnockDown2Texture_ = TextureManager::Load("playerTextures/LKnockDown2.png");
}

void Player::Update() {
	if (HP_ > 0){
    	Move();

    	// 攻撃入力チェック
    	if (input_->TriggerKey(DIK_J)) { // Jキーでパンチ
    		Attack();
    	}
	}

	AttackUpdate();

	TextureUpdate();
	worldTransform_.UpdateMatrix();
	worldTransformHitBox_.UpdateMatrix();
}

void Player::Draw(Camera& camera) { 
	model_->Draw(worldTransform_, camera, textureHandle_); 
	if (attackHitBox_.active) {
		worldTransformHitBox_.translation_ = attackHitBox_.pos;
		worldTransformHitBox_.scale_ = attackHitBox_.size;
		modelDebugHitBox_->Draw(worldTransformHitBox_, camera);
	}
}

void Player::Move() {

	move = {0, 0, 0};

	// 入力処理
	if (input_->PushKey(DIK_W))
		move.z += 1.0f;
	if (input_->PushKey(DIK_S))
		move.z -= 1.0f;
	if (input_->PushKey(DIK_A))
		move.x -= 1.0f;
	if (input_->PushKey(DIK_D))
		move.x += 1.0f;

	// ベクトル正規化
	if (move.x != 0.0f || move.z != 0.0f) {
		float length = std::sqrt(move.x * move.x + move.z * move.z);
		move.x /= length;
		move.z /= length;
	}

	// 向き更新（X方向に移動した場合のみ）
	if (move.x > 0.0f)
		facingDir_ = 1.0f;
	if (move.x < 0.0f)
		facingDir_ = -1.0f;

	// クールタイム減少
	if (stepTimer_ > 0)
		stepTimer_--;

	// ステップ開始判定
	if (stepTimer_ <= 0 && input_->TriggerKey(DIK_R) && (move.x != 0.0f || move.z != 0.0f)) {
		isStepping_ = true;
		stepDirection_ = move;
		stepTimer_ = stepCooldown_; // クールタイム開始
		stepFrame_ = 10;            // ステップ継続フレーム数
	}

	// ステップ中の処理
	if (isStepping_) {
		float stepSpeed = moveSpeed * stepPower_;
		worldTransform_.translation_.x += stepDirection_.x * stepSpeed;
		worldTransform_.translation_.z += stepDirection_.z * stepSpeed;

		stepFrame_--;
		if (stepFrame_ <= 0) {
			isStepping_ = false;
		}
		return; // ステップ中は通常移動を無効化
	}

	// 通常移動
	worldTransform_.translation_.x += move.x * moveSpeed;
	worldTransform_.translation_.z += move.z * moveSpeed;

	// 移動限界座標
	const float kStartMoveLimitX = 3.0f;
	const float kMoveLimitZ = 4.0f;
	const float kMinMoveLimitZ = 1.5f;

	// 範囲を越えない処理
	worldTransform_.translation_.x = std::max(worldTransform_.translation_.x, -kStartMoveLimitX);
	worldTransform_.translation_.z = std::max(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min(worldTransform_.translation_.z, +kMinMoveLimitZ);
}

void Player::Attack() {
	if (!canAttack_ || isAttacking_)
		return;

	isAttacking_ = true;
	canAttack_ = false;
	attackTimer_ = attackDuration_;

	// パンチテクスチャ切り替え（右左交互）
	attackFromRight_ = !attackFromRight_;

	// ヒットボックスはプレイヤーの向きに依存
	float hitboxOffsetX = 0.5f * facingDir_; // プレイヤーが右向きなら+0.8、左向きなら-0.8
	attackHitBox_.active = true;
	attackHitBox_.pos = worldTransform_.translation_ + Vector3{hitboxOffsetX, 0.1f, 0.0f};
	attackHitBox_.size = {0.2f, 0.5f, 0.2f};
}


void Player::AttackUpdate() {
	if (isAttacking_) {
		attackTimer_--;
		if (attackTimer_ <= 0) {
			isAttacking_ = false;
			attackHitBox_.active = false;
			attackCooldownTimer_ = attackCooldown_;
		} else {
			float hitboxOffsetX = 0.5f * facingDir_; // プレイヤーが右向きなら+0.8、左向きなら-0.8
			attackHitBox_.pos = worldTransform_.translation_ + Vector3{hitboxOffsetX, 0.1f, 0.0f};
		}
	}

	// クールタイム中
	else if (!canAttack_) {
		attackCooldownTimer_--;
		if (attackCooldownTimer_ <= 0) {
			canAttack_ = true;
		}
	}
}

void Player::TextureUpdate() {
	if (isAttacking_) {
		if (attackFromRight_) {
			if (facingDir_ == 1.0f) {
				textureHandle_ = RRightPunchTexture_;
			} else {
				textureHandle_ = LRightPunchTexture_;
			}
		} else {
			if (facingDir_ == 1.0f) {
				textureHandle_ = RLeftPunchTexture_;
			} else {
				textureHandle_ = LLeftPunchTexture_;
			}
		}
	} else if (HP_ <= 0) {
		knockDownTimer_ -= deltaTime;
		if (knockDownTimer_ <= 0) {
			isDead_ = true;
		} else if (knockDownTimer_ <= 1.0f) {
			if (facingDir_ == 1.0f) {
				textureHandle_ = RKnockDown2Texture_;
			} else {
				textureHandle_ = LKnockDown2Texture_;
			}
		} else {
			if (facingDir_ == 1.0f) {
				textureHandle_ = RKnockDownTexture_;
			} else {
				textureHandle_ = LKnockDownTexture_;
			}
		}

	} else {
		if (facingDir_ == 1.0f) {
			textureHandle_ = RPlayerTexture_;
		} else {
			textureHandle_ = LPlayerTexture_;
		}
	}
}

void Player::Reset() { 
	worldTransform_.translation_ = {0.0f, 1.0f, 0.0f};
	HP_ = 100;
	knockDownTimer_ = 2.0f;
	isDead_ = false;
	textureHandle_ = TextureManager::Load("playerTextures/RPlayer.png");
}

void Player::TakeDamage(int damage) {
	HP_ -= damage;
	if (HP_ < 0) {
		HP_ = 0;
	}
}
