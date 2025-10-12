#include "Player.h"
#include <cassert>
#include <algorithm>

using namespace KamataEngine;

void Player::Initialize(Model* model) { 
	input_ = Input::GetInstance(); 

	assert(model);
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_.y += 1.0f;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
}

void Player::Update() { 
	Move();

	worldTransform_.UpdateMatrix(); 
}

void Player::Draw(Camera& camera) { model_->Draw(worldTransform_, camera); }

void Player::Move() {
	Vector3 move = {0, 0, 0};
	float moveSpeed = 0.1f; // 通常移動速度

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
