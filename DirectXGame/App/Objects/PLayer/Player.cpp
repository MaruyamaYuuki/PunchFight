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
	nAttackPower_ = cfg_->getInt("Player.Attack.kNormalAttackPower");
	nAttackDuration_ = cfg_->getInt("Player.Attack.kNormalAttackDuration");
	nAttackCooldown_ = cfg_->getInt("Player.Attack.kNormalAttackCoolDown");
	nAttackHitBox_.size = cfg_->getVector3("Player.Attack.kNormalAttackHitBoxSize");
	worldTransformSP_.rotation_.x = cfg_->getFloat("Global.kPlaneModelRotateX");
	spAttackPower_ = cfg_->getInt("Player.Attack.kSPAttackPower");
	spAttackDuration_ = cfg_->getFloat("Player.Attack.kSPAttackDuration");
	spAttackCoolDown_ = cfg_->getFloat("Player.Attack.kSPAttackCoolDown");
	spAttackMoveSpeed_ = cfg_->getFloat("Player.Attack.kSPAttackMoveSpeed");
	spAttackHitBox_.size = cfg_->getVector3("Player.Attack.kSPAttackHitBoxSize");
	
	poseWaitTimer_ = cfg_->getFloat("Player.Clear.kPoseWaitTimer");
	knockDownDuration_ = cfg_->getFloat("Player.KnockDown.kKnockDownTimer");
	trailSpawnInterval_ = cfg_->getFloat("Player.Particle.kTrailSpawnInterval");
	smokeSize_ = cfg_->getVector3("Player.Particle.kSmokeSize");

	HP_ = maxHP_;
	worldTransform_.translation_ = kInitialPos_;
	knockDownTimer_ = knockDownDuration_;

	textureHandle_ = TextureManager::Load("playerTextures/RPlayer.png");

	// 右向きテクスチャ
	RPlayerTexture_ = TextureManager::Load("playerTextures/RPlayer.png");
	RLeftPunchTexture_ = TextureManager::Load("playerTextures/RLeftPunch.png");
	RRightPunchTexture_ = TextureManager::Load("playerTextures/RRightPunch.png");
	RUppercutTexture_ = TextureManager::Load("playerTextures/RUppercut.png");
	RKnockDownTexture_[0] = TextureManager::Load("playerTextures/RKnockDown.png");
	RKnockDownTexture_[1] = TextureManager::Load("playerTextures/RKnockDown2.png");
	RRunTexture_[0] = TextureManager::Load("playerTextures/RRun1.png");
	RRunTexture_[1] = TextureManager::Load("playerTextures/RRun2.png");
	RRunTexture_[2] = TextureManager::Load("playerTextures/RRun3.png");
	// 左向きテクスチャ
	LPlayerTexture_ = TextureManager::Load("playerTextures/LPlayer.png");
	LLeftPunchTexture_ = TextureManager::Load("playerTextures/LLeftPunch.png");
	LRightPunchTexture_ = TextureManager::Load("playerTextures/LRightPunch.png");
	LUppercutTexture_ = TextureManager::Load("playerTextures/LUppercut.png");
	LKnockDownTexture_[0] = TextureManager::Load("playerTextures/LKnockDown.png");
	LKnockDownTexture_[1] = TextureManager::Load("playerTextures/LKnockDown2.png");
	LRunTexture_[0] = TextureManager::Load("playerTextures/LRun1.png");
	LRunTexture_[1] = TextureManager::Load("playerTextures/LRun2.png");
	LRunTexture_[2] = TextureManager::Load("playerTextures/LRun3.png");
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
	PlayerCommand cmd = inputController_->GetCommand();

	if (HP_ > 0){
    	Move(cmd);

    	// 攻撃入力チェック
    	if (cmd.doAttack) { // Jキーでパンチ
    		Attack();
		} else if (cmd.doSpecialAttack) {
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
	worldTransformNormalAttackHitBox_.UpdateMatrix();
	worldTransformPHitBox_.UpdateMatrix();
	worldTransformSPHitBox_.UpdateMatrix();

	// ヒットボックスをプレイヤーの位置に追従
	playerHitBox_.pos = worldTransform_.translation_;
}

void Player::Draw(Camera& camera) { 
	model_->Draw(worldTransform_, camera, textureHandle_); 
	smokeManager_->Draw(camera);
	#ifdef _DEBUG
	if (nAttackHitBox_.active) {
		worldTransformNormalAttackHitBox_.translation_ = nAttackHitBox_.pos;
		worldTransformNormalAttackHitBox_.scale_ = nAttackHitBox_.size;
		modelDebugHitBox_->Draw(worldTransformNormalAttackHitBox_, camera);
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
		if (modelSPHitBox_) {
			worldTransformSPHitBox_.translation_ = spAttackHitBox_.pos;
			worldTransformSPHitBox_.scale_ = spAttackHitBox_.size;
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
	if (!isGoal_ && (isNormalAttacking_ || HP_ <= 0))
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

	if (isMoving && !isStepping_ && !isNormalAttacking_ && HP_ > 0) {
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

void Player::Attack() {
	if (!canNormalAttack_ || isNormalAttacking_)
		return;

	isNormalAttacking_ = true;
	canNormalAttack_ = false;
	nAttackTimer_ = nAttackDuration_;

	normalAttackSEVoiceHandle_ = audio_->PlayWave(normalAttackSEDataHandle_, false, 0.5f);

	// パンチテクスチャ切り替え（右左交互）
	nAttackFromRight_ = !nAttackFromRight_;

	// ヒットボックスはプレイヤーの向きに依存
	float hitboxOffsetX = 0.5f * facingDir_; // プレイヤーが右向きなら+0.8、左向きなら-0.8
	nAttackHitBox_.active = true;
	nAttackHitBox_.pos = worldTransform_.translation_ + Vector3{hitboxOffsetX, 0.1f, 0.0f};
	nAttackHitBox_.size = cfg_->getVector3("Player.Attack.kNormalAttackHitBoxSize");
}


void Player::AttackUpdate() {
	if (isNormalAttacking_) {
		nAttackTimer_--;
		if (nAttackTimer_ <= 0) {
			isNormalAttacking_ = false;
			nAttackHitBox_.active = false;
			nAttackCooldownTimer_ = nAttackCooldown_;
		} else {
			float hitboxOffsetX = 0.5f * facingDir_; // プレイヤーが右向きなら+0.8、左向きなら-0.8
			nAttackHitBox_.pos = worldTransform_.translation_ + Vector3{hitboxOffsetX, 0.1f, 0.0f};
		}
	}

	// クールタイム中
	else if (!canNormalAttack_) {
		nAttackCooldownTimer_--;
		if (nAttackCooldownTimer_ <= 0) {
			canNormalAttack_ = true;
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

	spAttackSEVoiceHandle_ = audio_->PlayWave(spAttackSEDataHandle_, false, 0.5f);

	// 発射時の向きを固定
	spAttackDirection_ = static_cast<float>(facingDir_);

	// --- 気弾モデルの初期位置 ---
	worldTransformSP_.translation_ = worldTransform_.translation_ + Vector3{spAttackDirection_ * 0.5f, 0.2f, 0.0f};
	worldTransformSP_.UpdateMatrix();

	// --- ヒットボックス初期化 ---
	spAttackHitBox_.active = true;
	spAttackHitBox_.pos = worldTransformSP_.translation_ + Vector3{spAttackDirection_ * 0.3f, 0.0f, 0.0f};
	spAttackHitBox_.size = cfg_->getVector3("Player.Attack.kSPAttackHitBoxSize");

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
	// 優先順位が高い順に判定し、確定したら return する
	if (isVictory_) {
		ApplyVictoryTexture();
		return;
	}
	if (HP_ <= 0) {
		ApplyDeathTexture();
		return;
	}
	if (isNormalAttacking_) {
		ApplyAttackTexture();
		return;
	}
	if (isStepping_) {
		ApplyStepTexture();
		return;
	}

	// それ以外（移動中か待機中）
	ApplyMovementTexture();
}

void Player::ApplyMovementTexture() { 
	bool isMoving = (move_.x != 0.0f || move_.z != 0.0f); 
	if (isMoving) {
		// 0,1,2,1 のループを配列で
		static const int walkPattern[4] = {0, 1, 2, 1};

		int texIndex = walkPattern[walkFrame_];
		textureHandle_ = (facingDir_ > 0) ? RRunTexture_[texIndex] : LRunTexture_[texIndex];
	} else {
		textureHandle_ = (facingDir_ > 0) ? RPlayerTexture_ : LPlayerTexture_;
	}
}

void Player::ApplyStepTexture() {
	textureHandle_ = (facingDir_ > 0) ? RRunTexture_[0] : LRunTexture_[0];
}

void Player::ApplyAttackTexture() {
	if (nAttackFromRight_) {
		textureHandle_ = (facingDir_ > 0) ? RRightPunchTexture_ : LRightPunchTexture_;
	} else {
		textureHandle_ = (facingDir_ > 0) ? RLeftPunchTexture_ : LLeftPunchTexture_;
	}
}

void Player::ApplyDeathTexture() {
	knockDownTimer_ -= deltaTime_;
	if (knockDownTimer_ <= 0) {
		isDead_ = true;
	} else if (knockDownTimer_ <= 1.0f) {
		textureHandle_ = (facingDir_ > 0) ? RKnockDownTexture_[1] : LKnockDownTexture_[1];
	} else {
		textureHandle_ = (facingDir_ > 0) ? RKnockDownTexture_[0] : LKnockDownTexture_[0];
	}
}

void Player::ApplyVictoryTexture() { 
	textureHandle_ = RUppercutTexture_; 
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
		hitSEVoiceHandle_ = audio_->PlayWave(hitSEDataHandle_, false, 0.5f);
	}
}
