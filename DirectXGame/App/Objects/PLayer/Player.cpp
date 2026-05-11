#define NOMINMAX
#include "Player.h"
#include <cassert>
#include <algorithm>
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;
using MyEngine::SmokeParticleManager;

void Player::Initialize(Model* model, KamataEngine::Model* modelSP, KamataEngine::Model* modelBox) { 
	input_ = Input::GetInstance(); 
	audio_ = Audio::GetInstance();

	assert(model);
	model_ = model;
	assert(modelSP);
	modelSpecial_ = modelSP;
	assert(modelBox);
	modelDebugHitBox_ = modelBox;
	modelHitBox_ = modelBox;
	modelSPHitBox_ = modelBox;

	worldTransform_.Initialize();
	worldTransformSP_.Initialize();
	worldTransformSPHitBox_.Initialize();
	worldTransformNormalAttackHitBox_.Initialize();
	worldTransformPHitBox_.Initialize();

	playerHitBox_.active = true;

	smokeManager_ = std::make_unique<SmokeParticleManager>();
	smokeManager_->Initialize();

	cfg_ = GameConfigManager::GetInstance();

	// 基本設定
	maxHP_ = cfg_->getInt("Player.kInitialHP");
	kInitialPos_ = cfg_->getVector3("Player.kGameInitialPos");
	worldTransform_.scale_ = cfg_->getVector3("Global.kPlaneModelScale");
	worldTransform_.rotation_.x = cfg_->getFloat("Global.kPlaneModelRotateX");
	moveSpeed_ = cfg_->getFloat("Player.kMoveSpeed");
	walkFrameInterval_ = cfg_->getInt("Player.kWalkFrameInterval");
	playerHitBox_.size = cfg_->getVector3("Player.kHitBoxSize");
	startMoveLimitX = cfg_->getFloat("Player.kStartMoveLimitX");
	moveLimitZ = cfg_->getFloat("Player.kMoveLimitZ");
	minMoveLimitZ = cfg_->getFloat("Player.kMinMoveLimitZ");

	stepCooldown_ = cfg_->getInt("Player.Step.kStepCoolDown");
	stepPower_ = cfg_->getFloat("Player.Step.kStepPower");
	worldTransformSP_.rotation_.x = cfg_->getFloat("Global.kPlaneModelRotateX");
	
	poseWaitTimer_ = cfg_->getFloat("Player.Clear.kPoseWaitTimer");
	knockDownDuration_ = cfg_->getFloat("Player.KnockDown.kKnockDownTimer");
	trailSpawnInterval_ = cfg_->getFloat("Player.Particle.kTrailSpawnInterval");
	smokeSize_ = cfg_->getVector3("Player.Particle.kSmokeSize");

	HP_ = maxHP_;
	worldTransform_.translation_ = kInitialPos_;
	knockDownTimer_ = knockDownDuration_;

	textureController_ = std::make_unique<PlayerTextureController>();
	textureController_->Initialize();

	combat_ = std::make_unique<PlayerCombat>();
	combat_->Initialize();

	textureHandle_ = TextureManager::Load("playerTextures/RPlayer.png");

	// 気弾テクスチャ
	SPTextureHandle_ = TextureManager::Load("playerTextures/RSpecial.png");
	RSpecialTexture_ = TextureManager::Load("playerTextures/RSpecial.png");
	LSpecialTexture_ = TextureManager::Load("playerTextures/LSpecial.png");

	smokeTexture_ = TextureManager::Load("effects/dust2.png");

	normalAttackSEDataHandle_ = audio_->LoadWave("audio/SE/punchSE.wav");
	spAttackSEDataHandle_ = audio_->LoadWave("audio/SE/spAttackSE.wav");
	hitSEDataHandle_ = audio_->LoadWave("audio/SE/hitSE.wav");
}

void Player::Update() {

	// ===== スタン処理 =====
	bool isStunned = (stunTimer_ > 0);
	if (isStunned) {
		stunTimer_--;
	}

	// ===== 入力取得 =====
	PlayerCommand cmd{};
	if (!isStunned) {
		cmd = inputController_->GetCommand();
	}

	// ===== 移動処理 =====
	if (!isStunned && HP_ > 0) {
		Move(cmd);
	}

	// ===== 攻撃入力（Combatへ委譲） =====
	if (!isStunned && HP_ > 0) {

		if (cmd.doAttack) {
			combat_->StartNormalAttack(worldTransform_.translation_, facingDir_);
		}

		if (cmd.doSpecialAttack) {
			combat_->StartSpecialAttack(worldTransform_.translation_, facingDir_);
		}
	}
	if (combat_->DidStartNormalAttack()) {
		normalAttackSEVoiceHandle_ = audio_->PlayWave(normalAttackSEDataHandle_, false, 0.5f);
		
	}
	if (combat_->DidStartSpecialAttack()) {
		spAttackSEVoiceHandle_ = audio_->PlayWave(spAttackSEDataHandle_, false, 0.5f);
        worldTransformSP_.translation_ = combat_->GetSpecialPos();
	}
	if (combat_->IsSpecialAttacking()) {
		SPTextureHandle_ = (combat_->GetSpecialDir() > 0) ? RSpecialTexture_ : LSpecialTexture_;
		worldTransformSP_.translation_ = combat_->GetSpecialPos();
	}

	// ===== Combat更新 =====
	combat_->Update(worldTransform_.translation_, facingDir_);

	// ===== エフェクト =====
	smokeManager_->Update(deltaTime_);

	// ===== テクスチャ更新 =====
	TextureUpdate();

	// ===== 行列更新 =====
	worldTransform_.UpdateMatrix();
	worldTransformSP_.UpdateMatrix();
	worldTransformNormalAttackHitBox_.UpdateMatrix();
	worldTransformPHitBox_.UpdateMatrix();
	worldTransformSPHitBox_.UpdateMatrix();

	// ===== ヒットボックス追従 =====
	playerHitBox_.pos = worldTransform_.translation_;
}

void Player::Draw(Camera& camera) { 
	model_->Draw(worldTransform_, camera, textureHandle_); 
	smokeManager_->Draw(camera);
	#ifdef _DEBUG
	if (combat_->GetNormalHitBox().active) {
		worldTransformNormalAttackHitBox_.translation_ = combat_->GetNormalHitBox().pos;
		worldTransformNormalAttackHitBox_.scale_ = combat_->GetNormalHitBox().size;
		modelDebugHitBox_->Draw(worldTransformNormalAttackHitBox_, camera);
	}
	if (playerHitBox_.active) {
		worldTransformPHitBox_.translation_ = playerHitBox_.pos;
		worldTransformPHitBox_.scale_ = playerHitBox_.size;
		modelHitBox_->Draw(worldTransformPHitBox_, camera);
	}
	#endif
}

void Player::DrawSpecial(KamataEngine::Camera& camera) {
	// --- 気弾の描画 ---
	if (combat_->IsSpecialAttacking()) {

		// 気弾本体
		modelSpecial_->Draw(worldTransformSP_, camera, SPTextureHandle_);

#ifdef _DEBUG
		// デバッグ用ヒットボックスモデル
		// （必要なら）
		if (modelSPHitBox_) {
			worldTransformSPHitBox_.translation_ = combat_->GetSpecialHitBox().pos;
			worldTransformSPHitBox_.scale_ = combat_->GetSpecialHitBox().size;
			worldTransformSPHitBox_.UpdateMatrix();

			modelSPHitBox_->Draw(worldTransformSPHitBox_, camera);
		}
#endif
	}
}

void Player::ClearAnimation() {
	const float targetX = 0.0f;
	PlayerCommand autoCmd;

	// ===== ゴールまでの移動ロジック =====
	if (!isGoal_) {
		if (worldTransform_.translation_.x < targetX) {
			autoCmd.moveDirection.x = 1.0f;
		} else {
			isGoal_ = true;
			// ゴール到達時に待ち時間をセット
			poseWaitTimer_ = 2.0f;
		}
	} else {
		// ===== ゴール後：数秒待ってポーズ =====
		autoCmd.moveDirection = {0, 0, 0};

		if (!isVictory_) {
			poseWaitTimer_ -= deltaTime_;
			if (poseWaitTimer_ <= 0.0f) {
				isVictory_ = true; // ★ ここで勝利ポーズ
			}
		}
	}

	// ===== 共通関数を呼ぶ =====
	Move(autoCmd);

	smokeManager_->Update(deltaTime_);

	TextureUpdate();
	worldTransform_.UpdateMatrix();
}



void Player::Move(const PlayerCommand& cmd) {
	// 移動不可フラグのチェック
	if (!isGoal_ && (combat_->IsNormalAttacking() || HP_ <= 0))
		return;

	// 入力コマンドを反映
	ApplyCommand(cmd);

	// 通常移動またはステップの座標反映
	if (isStepping_) {
		ApplyStepMovement();
	} else {
		ApplyNormalMovement();
	}

	// 仕上げ（制限、エフェクト、アニメ）
	ConstrainPosition();
	UpdateMoveEffects();
	UpdateAnimationFrames();
}

void Player::ApplyStepMovement() {
	float stepSpeed = moveSpeed_ * stepPower_;
	worldTransform_.translation_.x += stepDirection_.x * stepSpeed;
	worldTransform_.translation_.z += stepDirection_.z * stepSpeed;

	stepFrame_--;
	if (stepFrame_ <= 0) {
		isStepping_ = false;
	}
}

void Player::ApplyNormalMovement() {

	worldTransform_.translation_.x += move_.x * moveSpeed_;
	worldTransform_.translation_.z += move_.z * moveSpeed_;
}

void Player::ConstrainPosition() {
	if (!enableMoveLimit_)
		return;
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -startMoveLimitX, endMoveLimitX_);
	worldTransform_.translation_.z = std::clamp(worldTransform_.translation_.z, -moveLimitZ, minMoveLimitZ);
}

void Player::UpdateMoveEffects() {
	// 移動していないなら何もしない
	if (move_.x == 0.0f && move_.z == 0.0f)
		return;

	trailSpawnTimer_ -= deltaTime_;
	if (trailSpawnTimer_ <= 0.0f) {
		trailSpawnTimer_ = trailSpawnInterval_;

		if (smokeManager_) {
			Vector3 pos = worldTransform_.translation_;
			pos.y -= 0.4f;
			smokeManager_->SetTexture(smokeTexture_);
			smokeManager_->Spawn(pos, smokeSize_);
		}
	}
}

void Player::UpdateAnimationFrames() {
	bool isMoving = (move_.x != 0.0f || move_.z != 0.0f);

	if (isMoving && !isStepping_ && !combat_->IsNormalAttacking() && HP_ > 0) {
		walkFrameTimer_++;
		if (walkFrameTimer_ >= walkFrameInterval_) {
			walkFrameTimer_ = 0;
			walkFrame_ = (walkFrame_ + 1) % 4;
		}
	} else {
		walkFrame_ = 0;
		walkFrameTimer_ = 0;
	}
}

void Player::TextureUpdate() {

	PlayerState state;

	if (stunTimer_ > 0) {
		state = PlayerState::Stun;
	} else if (isVictory_) {
		state = PlayerState::Victory;
	} else if (HP_ <= 0) {
		knockDownTimer_ -= deltaTime_;
		if (knockDownTimer_ <= 0) {
			isDead_ = true;
		} 
		state = PlayerState::Dead;
	} else if (combat_->IsNormalAttacking()) {
		state = PlayerState::Attack;
	} else if (isStepping_) {
		state = PlayerState::Step;
	} else if (move_.x != 0.0f || move_.z != 0.0f) {
		state = PlayerState::Move;
	} else {
		state = PlayerState::Idle;
	}

	textureHandle_ = textureController_->GetTexture(facingDir_ > 0, state, walkFrame_, combat_->GetAttackForRight(), knockDownTimer_);
}

void Player::ApplyCommand(const PlayerCommand& cmd) {
	// 1. 移動方向ベクトルのセット
	move_ = cmd.moveDirection;

	// 2. 移動入力がある場合のみ正規化と向きの更新
	if (move_.x != 0.0f || move_.z != 0.0f) {
		float length = std::sqrt(move_.x * move_.x + move_.z * move_.z);
		move_.x /= length;
		move_.z /= length;

		// 向きの更新
		if (move_.x > 0.0f)
			facingDir_ = 1.0f;
		else if (move_.x < 0.0f)
			facingDir_ = -1.0f;
	}

	// 3. ステップ（回避）の開始判定
	// クールタイム減少
	if (stepTimer_ > 0)
		stepTimer_--;

	// 開始条件：クールタイム終了 且つ ステップボタン 且つ 移動中
	if (stepTimer_ <= 0 && cmd.doStep && (move_.x != 0.0f || move_.z != 0.0f)) {
		isStepping_ = true;
		stepDirection_ = move_; // 現在の移動方向へステップ
		stepTimer_ = stepCooldown_;
		stepFrame_ = 10; // 10フレーム継続
	}
}

void Player::Reset() { 
	worldTransform_.translation_ = kInitialPos_;
	HP_ = maxHP_;
	knockDownTimer_ = knockDownDuration_;
	isDead_ = false;
	textureHandle_ = TextureManager::Load("playerTextures/RPlayer.png");
}

void Player::UpdateWorldTransform() {
	worldTransform_.UpdateMatrix();
	worldTransformNormalAttackHitBox_.UpdateMatrix();
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
	} else {
		stunTimer_ = 10; // 例：10フレーム
		hitSEVoiceHandle_ = audio_->PlayWave(hitSEDataHandle_, false, 0.5f);
	}
}
