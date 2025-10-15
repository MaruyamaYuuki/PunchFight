#include "Player.h"
#include <cassert>
#include <algorithm>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void Player::Initialize(Model* model) { 
	input_ = Input::GetInstance(); 

	assert(model);
	model_ = model;

	modelDebugHitBox_ = Model::Create();

	worldTransform_.Initialize();
	worldTransform_.translation_.y += 1.0f;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformHitBox_.Initialize();
}

void Player::Update() {
	Move();

	// 攻撃入力チェック
	if (input_->TriggerKey(DIK_J)) { // Jキーでパンチ
		Attack();
	}

	AttackUpdate();
	worldTransform_.UpdateMatrix();
	worldTransformHitBox_.UpdateMatrix();
}

void Player::Draw(Camera& camera) { 
	model_->Draw(worldTransform_, camera); 
	if (hitBox_.active) {
		worldTransformHitBox_.translation_ = hitBox_.pos;
		worldTransformHitBox_.scale_ = hitBox_.size;
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
	hitBox_.active = true;
	hitBox_.pos = worldTransform_.translation_ + Vector3{hitboxOffsetX, 0.1f, 0.0f};
	hitBox_.size = {0.2f, 0.5f, 0.2f};
}


void Player::AttackUpdate() {
	if (isAttacking_) {
		attackTimer_--;
		if (attackTimer_ <= 0) {
			isAttacking_ = false;
			hitBox_.active = false;
			attackCooldownTimer_ = attackCooldown_;
		} else {
			float hitboxOffsetX = 0.5f * facingDir_; // プレイヤーが右向きなら+0.8、左向きなら-0.8
			hitBox_.pos = worldTransform_.translation_ + Vector3{hitboxOffsetX, 0.1f, 0.0f};
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
