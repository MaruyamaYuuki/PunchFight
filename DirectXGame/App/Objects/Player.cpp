#define NOMINMAX
#include "Player.h"
#include <cassert>
#include <algorithm>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void Player::Initialize(Model* model, KamataEngine::Model* modelSP, KamataEngine::Model* modelBox, KamataEngine::Vector3 pos) { 
	input_ = Input::GetInstance(); 

	assert(model);
	model_ = model;
	assert(modelSP);
	modelSpecial_ = modelSP;
	assert(modelBox);
	modelDebugHitBox_ = modelBox;
	modelHitBox_ = modelBox;
	modelSPHiyBox_ = modelBox;

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.rotation_.x = 0.75f;
	worldTransformSP_.Initialize();
	worldTransformHitBox_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformSPHitBox_.Initialize();
	worldTransformHitBox_.Initialize();
	worldTransformPHitBox_.Initialize();

	playerHitBox_.active = true;
	playerHitBox_.size = {0.3f, 0.5f, 0.3f};

	smokeManager_ = std::make_unique<SmokeParticleManager>();
	smokeManager_->Initialize();

	textureHandle_ = TextureManager::Load("playerTextures/RPlayer.png");

	// 右向きテクスチャ
	RPlayerTexture_ = TextureManager::Load("playerTextures/RPlayer.png");
	RLeftPunchTexture_ = TextureManager::Load("playerTextures/RLeftPunch.png");
	RRightPunchTexture_ = TextureManager::Load("playerTextures/RRightPunch.png");
	RUppercutTexture_ = TextureManager::Load("playerTextures/RUppercut.png");
	RKnockDownTexture_ = TextureManager::Load("playerTextures/RKnockDown.png");
	RKnockDown2Texture_ = TextureManager::Load("playerTextures/RKnockDown2.png");
	RRunTexture1_ = TextureManager::Load("playerTextures/RRun1.png");
	RRunTexture2_ = TextureManager::Load("playerTextures/RRun2.png");
	RRunTexture3_ = TextureManager::Load("playerTextures/RRun3.png");
	// 左向きテクスチャ
	LPlayerTexture_ = TextureManager::Load("playerTextures/LPlayer.png");
	LLeftPunchTexture_ = TextureManager::Load("playerTextures/LLeftPunch.png");
	LRightPunchTexture_ = TextureManager::Load("playerTextures/LRightPunch.png");
	LUppercutTexture_ = TextureManager::Load("playerTextures/LUppercut.png");
	LKnockDownTexture_ = TextureManager::Load("playerTextures/LKnockDown.png");
	LKnockDown2Texture_ = TextureManager::Load("playerTextures/LKnockDown2.png");
	LRunTexture1_ = TextureManager::Load("playerTextures/LRun1.png");
	LRunTexture2_ = TextureManager::Load("playerTextures/LRun2.png");
	LRunTexture3_ = TextureManager::Load("playerTextures/LRun3.png");
	// 気弾テクスチャ
	SPTextureHandle_ = TextureManager::Load("playerTextures/RSpecial.png");
	RSpecialTexture_ = TextureManager::Load("playerTextures/RSpecial.png");
	LSpecialTexture_ = TextureManager::Load("playerTextures/LSpecial.png");

	smokeTexture_ = TextureManager::Load("effects/dust2.png");
}

void Player::Update() {

	if (input_->TriggerKey(DIK_P)) {
		worldTransform_.rotation_.x += 0.1f;
		DebugText::GetInstance()->ConsolePrintf("Player X Rotation: %f\n", worldTransform_.rotation_.x);
	}

	if (HP_ > 0){
    	Move();

    	// 攻撃入力チェック
    	if (input_->TriggerKey(DIK_J)) { // Jキーでパンチ
    		Attack();
		} else if (input_->TriggerKey(DIK_K)) {
			SpecialAttack();
		}
	}

	justSpecialAttacked_ = false;

	AttackUpdate();
	SpecialAttackUpdate();

	smokeManager_->Update(deltaTime_);

	TextureUpdate();
	worldTransform_.UpdateMatrix();
	worldTransformSP_.UpdateMatrix();
	worldTransformHitBox_.UpdateMatrix();
	worldTransformPHitBox_.UpdateMatrix();
	worldTransformSPHitBox_.UpdateMatrix();

	// ヒットボックスをプレイヤーの位置に追従
	playerHitBox_.pos = worldTransform_.translation_;
}

void Player::Draw(Camera& camera) { 
	model_->Draw(worldTransform_, camera, textureHandle_); 
	smokeManager_->Draw(camera);
	#ifdef _DEBUG
	if (attackHitBox_.active) {
		worldTransformHitBox_.translation_ = attackHitBox_.pos;
		worldTransformHitBox_.scale_ = attackHitBox_.size;
		modelDebugHitBox_->Draw(worldTransformHitBox_, camera);
	}
	if (playerHitBox_.active) {
		worldTransformPHitBox_.translation_ = playerHitBox_.pos;
		worldTransformPHitBox_.scale_ = playerHitBox_.size;
		modelHitBox_->Draw(worldTransformPHitBox_, camera);
	}
	#endif
	// --- 気弾の描画 ---
	if (isSpecialAttacking_) {

		// 気弾本体
		modelSpecial_->Draw(worldTransformSP_, camera, SPTextureHandle_);

		#ifdef _DEBUG
		// デバッグ用ヒットボックスモデル
		// （必要なら）
		if (modelSPHiyBox_) {
			worldTransformSPHitBox_.translation_ = spAttackHitBox_.pos;
			worldTransformSPHitBox_.scale_ = spAttackHitBox_.size;
			worldTransformSPHitBox_.UpdateMatrix();

			modelSPHiyBox_->Draw(worldTransformSPHitBox_, camera);
		}
		#endif
	}
}

void Player::ClearAnimation(bool isSpot) {
	// ここでゴール地点を指定
	const float targetX = 0.0f;

	// --- まだゴールしてない時だけ移動 ---
	if (!isGoal_) {

		// 目的地にまだ着いていないなら右へ移動（今回は右向き想定）
		if (worldTransform_.translation_.x < targetX) {
			move_.x = 1.0f; // 右へ移動
		} else {
			move_.x = 0.0f; // 到達したら停止

			// ★ ポーズに入るため勝利フラグON
			isGoal_ = true;
		}

		// --- ベクトル正規化 ---
		if (move_.x != 0.0f || move_.z != 0.0f) {
			float length = std::sqrt(move_.x * move_.x + move_.z * move_.z);
			move_.x /= length;
			move_.z /= length;
		}

		// --- 向き ---
		if (move_.x > 0.0f)
			facingDir_ = 1.0f;
		if (move_.x < 0.0f)
			facingDir_ = -1.0f;

		// --- 移動 ---
		worldTransform_.translation_.x += move_.x * moveSpeed_;

	} else {
		// ★ 勝利フラグ時は移動しない
		move_.x = 0.0f;
	}

	// --- アニメ処理 ---
	bool isMoving = (move_.x != 0.0f || move_.z != 0.0f);

	if (isMoving && !isStepping_ && !isAttacking_ && HP_ > 0 && !isVictory_) {
		walkFrameTimer_++;

		if (walkFrameTimer_ >= walkFrameInterval_) {
			walkFrameTimer_ = 0;
			walkFrame_ = (walkFrame_ + 1) % 4; // 4枚ループ
		}
	} else {
		// 止まったら初期フレーム
		walkFrame_ = 0;
		walkFrameTimer_ = 0;
	}

	if (isGoal_ && isSpot && !isVictory_) {
		if (poseWaitTimer_ > 0) {
			poseWaitTimer_ -= deltaTime_;
		} else if(poseWaitTimer_<=0) {
			poseWaitTimer_ = 0;
			isVictory_ = true;
		}
	}

	TextureUpdate();
	worldTransform_.UpdateMatrix();
}


void Player::Move() {
	// 攻撃中なら移動をキャンセル
	if (isAttacking_) {
		return;
	}

	move_ = {0, 0, 0};

	// 入力処理
	if (input_->PushKey(DIK_W))
		move_.z += 1.0f;
	if (input_->PushKey(DIK_S))
		move_.z -= 1.0f;
	if (input_->PushKey(DIK_A))
		move_.x -= 1.0f;
	if (input_->PushKey(DIK_D))
		move_.x += 1.0f;

	// ベクトル正規化
	if (move_.x != 0.0f || move_.z != 0.0f) {
		float length = std::sqrt(move_.x * move_.x + move_.z * move_.z);
		move_.x /= length;
		move_.z /= length;

        // 一定間隔でパーティクル生成
		trailSpawnTimer_ -= deltaTime_;
		if (trailSpawnTimer_ <= 0.0f) {
			trailSpawnTimer_ = trailSpawnInterval_; // 例: 0.05f

			if (smokeManager_) {
				Vector3 pos = worldTransform_.translation_;
				pos.y -= 0.4f; // 足元に出す場合
				smokeManager_->SetTexture(smokeTexture_);
				smokeManager_->Spawn(pos, smokeSize_);
			}
		}
	}

	// 向き更新（X方向に移動した場合のみ）
	if (move_.x > 0.0f)
		facingDir_ = 1.0f;
	if (move_.x < 0.0f)
		facingDir_ = -1.0f;

	// クールタイム減少
	if (stepTimer_ > 0)
		stepTimer_--;

	// ステップ開始判定
	if (stepTimer_ <= 0 && input_->TriggerKey(DIK_H) && (move_.x != 0.0f || move_.z != 0.0f)) {
		isStepping_ = true;
		stepDirection_ = move_;
		stepTimer_ = stepCooldown_; // クールタイム開始
		stepFrame_ = 10;            // ステップ継続フレーム数
	}

	// ステップ中の処理
	if (isStepping_) {
		float stepSpeed = moveSpeed_ * stepPower_;
		worldTransform_.translation_.x += stepDirection_.x * stepSpeed;
		worldTransform_.translation_.z += stepDirection_.z * stepSpeed;

		stepFrame_--;
		if (stepFrame_ <= 0) {
			isStepping_ = false;
		}
		return; // ステップ中は通常移動を無効化
	}

	// 通常移動
	worldTransform_.translation_.x += move_.x * moveSpeed_;
	worldTransform_.translation_.z += move_.z * moveSpeed_;

	// 移動限界座標
	const float kStartMoveLimitX = 3.0f;
	const float kMoveLimitZ = 4.0f;
	const float kMinMoveLimitZ = 1.5f;

	// 範囲を越えない処理
	worldTransform_.translation_.x = std::max(worldTransform_.translation_.x, -kStartMoveLimitX);
	worldTransform_.translation_.x = std::min(worldTransform_.translation_.x, +endMoveLimitX_);
	worldTransform_.translation_.z = std::max(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min(worldTransform_.translation_.z, +kMinMoveLimitZ);

	bool isMoving = (move_.x != 0.0f || move_.z != 0.0f);

	// 歩行アニメ進行
	if (isMoving && !isStepping_ && !isAttacking_ && HP_ > 0) {
		walkFrameTimer_++;

		if (walkFrameTimer_ >= walkFrameInterval_) {
			walkFrameTimer_ = 0;
			walkFrame_ = (walkFrame_ + 1) % 4; // 4枚ループ
		}
	} else {
		// 止まったらフレームリセットしてもOK（お好み）
		walkFrame_ = 0;
		walkFrameTimer_ = 0;
	}
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
	attackHitBox_.size = {0.2f, 0.5f, 1.5f};
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

void Player::SpecialAttack() {

	if (!canSpecialAttack_ || isSpecialAttacking_)
		return;

	isSpecialAttacking_ = true;
	canSpecialAttack_ = false;

	// --- クールタイムを即スタート ---
	spAttackCooldownTimer_ = spAttackCoolDown_;

	spAttackTimer_ = spAttackDuration_;

	// 発射時の向きを固定
	spAttackDirection_ = static_cast<float>(facingDir_);

	// --- 気弾モデルの初期位置 ---
	worldTransformSP_.translation_ = worldTransform_.translation_ + Vector3{spAttackDirection_ * 0.5f, 0.2f, 0.0f};
	worldTransformSP_.UpdateMatrix();

	// --- ヒットボックス初期化 ---
	spAttackHitBox_.active = true;
	spAttackHitBox_.pos = worldTransformSP_.translation_ + Vector3{spAttackDirection_ * 0.3f, 0.0f, 0.0f};
	spAttackHitBox_.size = {0.5f, 0.5f, 1.5f};

	// --- テクスチャ切替 ---
	SPTextureHandle_ = (spAttackDirection_ > 0) ? RSpecialTexture_ : LSpecialTexture_;
}

void Player::SpecialAttackUpdate() {

	// ---------------------
	// 気弾の更新
	// ---------------------
	if (isSpecialAttacking_) {

		spAttackTimer_ -= deltaTime_;

		// 発射時の向きで移動
		worldTransformSP_.translation_.x += spAttackMoveSpeed_ * spAttackDirection_;
		worldTransformSP_.UpdateMatrix();

		// ヒットボックス追従
		spAttackHitBox_.pos = worldTransformSP_.translation_ + Vector3{spAttackDirection_ * 0.3f, 0.0f, 0.0f};

		// 気弾が消える
		if (spAttackTimer_ <= 0.0f) {
			isSpecialAttacking_ = false;
			spAttackHitBox_.active = false;
		}
	}

	// ---------------------
	// クールタイム進行（気弾が生きていても進む）
	// ---------------------
	if (!canSpecialAttack_) {
		spAttackCooldownTimer_ -= deltaTime_;

		if (spAttackCooldownTimer_ <= 0.0f) {
			canSpecialAttack_ = true;
			spAttackCooldownTimer_ = 0.0f;
		}
	}
}


void Player::TextureUpdate() {	
	bool isMoving = (move_.x != 0.0f || move_.z != 0.0f);
	// 勝利ポーズ
	if (isVictory_) {
		textureHandle_ = RUppercutTexture_;
	}
	// 攻撃
	else if (isAttacking_) {
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
	}  
	// ダウン
	else if (HP_ <= 0) {
		knockDownTimer_ -= deltaTime_;
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

	} 
	// ステップ
	else if (isStepping_) {
		if (facingDir_ == 1.0f) {
			textureHandle_ = RRunTexture1_;
		} else {
			textureHandle_ = LRunTexture1_;
		}
		return;
	}
	// 移動
	else if (isMoving) {
		if (facingDir_ == 1.0f) {
			switch (walkFrame_) {
			case 0:
				textureHandle_ = RRunTexture1_;
				break;
			case 1:
				textureHandle_ = RRunTexture2_;
				break;
			case 2:
				textureHandle_ = RRunTexture3_;
				break;
			case 3:
				textureHandle_ = RRunTexture2_;
				break;
			}
		} else {
			switch (walkFrame_) {
			case 0:
				textureHandle_ = LRunTexture1_;
				break;
			case 1:
				textureHandle_ = LRunTexture2_;
				break;
			case 2:
				textureHandle_ = LRunTexture3_;
				break;
			case 3:
				textureHandle_ = LRunTexture2_;
				break;
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

void Player::UpdateWorldTransform() {
	worldTransform_.UpdateMatrix();
	worldTransformHitBox_.UpdateMatrix();
	worldTransformPHitBox_.UpdateMatrix();

	// ヒットボックスをプレイヤーの位置に追従
	playerHitBox_.pos = worldTransform_.translation_;
}

void Player::OnHit(int32_t damage) {
	if (!isStepping_) {
    	HP_ -= damage;
	}
	if (HP_ < 0) {
		HP_ = 0;
	}
}
