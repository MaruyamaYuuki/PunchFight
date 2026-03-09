#include "EnemyBase.h"
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;
using MyEngine::SmokeParticleManager;
using MyEngine::DustParticleManager;

void EnemyBase::Initialize(const EnemyData& data) {
	cfg_ = GameConfigManager::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::CreateFromOBJ(data.modelPath, true);
	speed_ = data.speed;
	hp_ = data.hp;
	attackPower_ = data.attackPower;

	modelEHitBox_ = Model::CreateFromOBJ("boxFrame", true);
	modelAHitBox_ = Model::CreateFromOBJ("boxFrame", true);

	worldTransform_.Initialize();
	worldTransform_.rotation_.x = 0.75f;
	worldTransformEHitBox_.Initialize();
	worldTransformAHitBox_.Initialize();

	smokeManager_ = std::make_unique<SmokeParticleManager>();
	smokeManager_->Initialize();

	dustManager_ = std::make_unique<DustParticleManager>();
	dustManager_->Initialize();

	worldTransform_.rotation_.x = cfg_->getFloat("Global.kPlaneModelRotateX");
	walkFrameInterval_ = cfg_->getInt("Enemy.Default.kWalkFrameInterval");
	gravity_ = cfg_->getFloat("Enemy.Default.kGravityAcceleration");
	knockbackDuration_ = cfg_->getFloat("Enemy.Default.kKnockbackDuration");
	attackDuration_ = cfg_->getFloat("Enemy.Default.kAttackDuration");
	attackCooldown_ = cfg_->getFloat("Enemy.Default.kAttackCooldown");
	attackRange_ = cfg_->getFloat("Enemy.Default.kAttackRange");
	stunDuration_ = cfg_->getFloat("Enemy.Default.kStunDuration");
	stunShakeAmplitude_ = cfg_->getFloat("Enemy.Default.kStunShakeAmplitude");
	stunShakeSpeed_ = cfg_->getFloat("Enemy.Default.kStunShakeSpeed");
	smokeSpawnInterval_ = cfg_->getFloat("Enemy.Default.kSmokeSpawnInterval");
	smokeSize_ = cfg_->getVector3("Enemy.Default.kSmokeSize");

	attackSEDataHandle_ = audio_->LoadWave("audio/SE/punchSE.wav");
	hitSEDataHandle_ = audio_->LoadWave("audio/SE/hitSE.wav");
	blownSEDataHandle_ = audio_->LoadWave("audio/SE/blownSE.wav");
}

void EnemyBase::Update(const Vector3&, const std::vector<std::unique_ptr<EnemyBase>>&) {
	if (isKnockBack_) {

		knockbackTime_ += deltaTime_;

        // ======== パーティクル生成 ========
		smokeSpawnTimer_ -= deltaTime_;
		if (smokeSpawnTimer_ <= 0.0f) {
			smokeSpawnTimer_ = smokeSpawnInterval_; // 次の生成までの時間

			smokeManager_->Spawn(worldTransform_.translation_, smokeSize_);
		}

		// ======== ノックバック物理処理 ========
		// 減速用の係数
		float drag = cfg_->getFloat("Enemy.Default.kKnockbackDragFactor");

		// X方向に加速して減速していく
		knockbackVelocity_.x *= drag;
		worldTransform_.translation_.x += knockbackVelocity_.x * deltaTime_;

		// Y方向は重力で落下させる
		knockbackVelocity_.y -= gravity_ * deltaTime_;
		worldTransform_.translation_.y += knockbackVelocity_.y * deltaTime_;

		// 規定時間で消滅
		if (knockbackTime_ >= knockbackDuration_) {
			isKnockBack_ = false;
			isDead_ = true;
		}
	}

	// ===== スタン処理 =====
	if (isStun_ && hp_ > 0) {
		stunTimer_ -= deltaTime_;
		stunShakeTime_ += deltaTime_;

		float shakeOffset = std::sin(stunShakeTime_ * stunShakeSpeed_) * stunShakeAmplitude_;

		// 元の位置に振動を加える
		worldTransform_.translation_.x = originalPosition_.x + shakeOffset;


		if (stunTimer_ <= 0.0f) {
			isStun_ = false;
			worldTransform_.translation_ = originalPosition_;
		}
	}

    // ======== パーティクル更新 ========
	smokeManager_->Update(deltaTime_);
	dustManager_->Update(deltaTime_);

	UpdateTextures();
	worldTransform_.UpdateMatrix();
	worldTransformEHitBox_.UpdateMatrix();
	worldTransformAHitBox_.UpdateMatrix();
	hitBox_.pos = worldTransform_.translation_;

}

void EnemyBase::Draw(Camera& camera) {
	smokeManager_->Draw(camera);
	if (model_) {
		model_->Draw(worldTransform_, camera, textureHandle_);
	}
	dustManager_->Draw(camera);
	#ifdef _DEBUG
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
	#endif
}

void EnemyBase::OnHit(int32_t damage, const Vector3& attackDir) {
	// ====== ダストパーティクル発生 ======
	if (dustManager_) {
		dustManager_->Spawn(worldTransform_.translation_);
	}

	hp_ -= damage;


	if (hp_ <= 0 && !isKnockBack_) {
		blownSEVoiceHandle_ = audio_->PlayWave(blownSEDataHandle_, false, 0.5f);

		hp_ = 0;
		isKnockBack_ = true;
		knockbackTime_ = 0.0f;

		// スマブラ風 初速
		float basePower = cfg_->getFloat("Enemy.Default.kKnockbackBasePower"); // 吹っ飛び強さ
		float upwardBoost = cfg_->getFloat("Enemy.Default.kKnockbackUpwardBoost"); // 上方向の初速

		knockbackVelocity_ = {attackDir.x * basePower, upwardBoost, 0.0f};
	} else {
		hitSEVoiceHandle_ = audio_->PlayWave(hitSEDataHandle_, false, 0.5f);

    	isStun_ = true;
     	stunTimer_ = stunDuration_;
    	stunShakeTime_ = 0.0f;
    	originalPosition_ = worldTransform_.translation_;
	}
}

void EnemyBase::SetHitBox(const Vector3& center, const Vector3& size) {
	hitBox_.pos = center;
	hitBox_.size = size;
	hitBox_.active = true;
}

void EnemyBase::SetAttackHitBox(const Vector3& pos) {
	attackHitBox_.pos = pos;
	attackHitBox_.size = {0.2f, 0.5f, 0.5f};
	attackHitBox_.active = true;
}

void EnemyBase::SetPosition(float x, float y, float z) { worldTransform_.translation_ = {x, y, z}; }

void EnemyBase::SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

Vector3 EnemyBase::ComputeSeparation(const std::vector<std::unique_ptr<EnemyBase>>& allEnemies, float separationDistance) {
	Vector3 offset{0.0f, 0.0f, 0.0f};

	for (auto& other : allEnemies) {
		if (other.get() == this)
			continue;

		Vector3 toOther = worldTransform_.translation_ - other->GetPosition();
		float dist = std::sqrt(toOther.x * toOther.x + toOther.z * toOther.z); // xz平面

		if (dist < separationDistance && dist > 0.001f) {
			// 正規化して距離に応じて押し戻す
			toOther.z /= dist;

			float pushFactor = separationDistance - dist;
			offset.z += toOther.z * pushFactor;
		}
	}

	return offset;
}

void EnemyBase::MoveTowardPlayer(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {
	Vector3 moveDir{0, 0, 0};

	// プレイヤー方向ベクトル計算
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float len = sqrtf(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);

	if (len > 0.001f) {
		toPlayer.x /= len;
		toPlayer.z /= len;
	}

	// 分離処理
	Vector3 sep = ComputeSeparation(allEnemies, 1.0f);
	toPlayer.x += sep.x;
	toPlayer.z += sep.z;

	// 正規化
	float finalLen = sqrtf(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
	if (finalLen > 0.001f) {
		toPlayer.x /= finalLen;
		toPlayer.z /= finalLen;
	}

	// 移動
	worldTransform_.translation_.x += toPlayer.x * speed_;
	worldTransform_.translation_.z += toPlayer.z * speed_;

	// 向き
	if (moveDir.x > 0.01f) facingDir_ = 1.0f;
	if (moveDir.x < -0.01f) facingDir_ = -1.0f;

	// 歩行アニメ進行
	bool isMoving = (finalLen > 0.001f);
	if (isMoving && !isAttacking_ && hp_ > 0) {
		walkFrameTimer_++;

		if (walkFrameTimer_ >= walkFrameInterval_) {
			walkFrameTimer_ = 0;
			walkFrame_ = (walkFrame_ + 1) % 4;
		} 
	}else {
			walkFrame_ = 0;
			walkFrameTimer_ = 0;
	}
}

void EnemyBase::DoNormalAttack(const Vector3& playerPos) {
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// ===== 一定距離以内なら攻撃モードON（離れてもOFFにしない） =====
	if (!isAttackMode_ && dist <= attackRange_) {
		isAttackMode_ = true;

		// 向きを固定する
		attackDirX_ = (toPlayer.x >= 0) ? 1.0f : -1.0f;
		facingDir_ = attackDirX_;
	}

	// 攻撃モードじゃないなら何もしない
	if (!isAttackMode_) {
		return;
	}

	// ===== クールタイム =====
	if (attackCooldownTimer_ > 0.0f) {
		attackCooldownTimer_ -= deltaTime_;
		return; // 攻撃できないのでここで終了
	}

	// ===== 攻撃中処理 =====
	if (isAttacking_) {
		attackTimer_ -= deltaTime_;

		facingDir_ = attackDirX_;

		if (attackTimer_ <= 0) {
			// 攻撃終了
			isAttacking_ = false;
			attackHitBox_.active = false;
			hasDealtDamage_ = false;
			attackCooldownTimer_ = attackCooldown_;
			isAttackMode_ = false;
		} else {
			// 攻撃中：ヒットボックス追従
			float offsetX = 0.5f * facingDir_;
			SetAttackHitBox(worldTransform_.translation_ + Vector3{offsetX, 0.1f, 0});
		}

		return;
	}

	// ===== 攻撃開始 =====
	isAttacking_ = true;
	attackTimer_ = attackDuration_;
	hasDealtDamage_ = false;

    attackSEVoiceHandle_ = audio_->PlayWave(attackSEDataHandle_, false, 0.5f);

	float offsetX = 0.5f * facingDir_;
	SetAttackHitBox(worldTransform_.translation_ + Vector3{offsetX, 0.1f, 0});
	attackHitBox_.active = true;
}

void EnemyBase::UpdateTextures() {
	switch (state_) {
	case EnemyState::Idle:
		textureHandle_ = (facingDir_ > 0) ? RIdleTexture_ : LIdleTexture_;
		break;
	case EnemyState::Walking: {
		// 0,1,2,3 のループを配列で
		static const int walkPattern[4] = {0, 1, 2, 3};

		int texIndex = walkPattern[walkFrame_];
		textureHandle_ = (facingDir_ > 0) ? RWalkTexture_[texIndex] : LWalkTexture_[texIndex];
		break;
	}
	case EnemyState::AttackWait:
		textureHandle_ = (facingDir_ > 0) ? RWaitTexture_ : LWaitTexture_;
		break;
	case EnemyState::Attacking:
		textureHandle_ = (facingDir_ > 0) ? RAttackTexture_ : LAttackTexture_;
		break;
	case EnemyState::Stunned:
		textureHandle_ = (facingDir_ > 0) ? RStunTexture_ : LStunTexture_;
		break;
	case EnemyState::Knockback:
		textureHandle_ = (facingDir_ > 0) ? RStunTexture_ : LStunTexture_;
		break;
	case EnemyState::Dead:
			textureHandle_ = (facingDir_ > 0) ? RStunTexture_ : LStunTexture_;
		break;
	}
}

bool EnemyBase::IsMovementInterrupted() const {
	// 基本ルール：ノックバック中、スタン中、死亡時は動きを止める
	return isKnockBack_ || isStun_ || hp_ <= 0;
}

void EnemyBase::UpdateBasicState() {
	// 共通のステータス更新処理
	if (isKnockBack_)
		state_ = EnemyState::Knockback;
	else if (hp_ <= 0)
		state_ = EnemyState::Dead;
	else if (isStun_)
		state_ = EnemyState::Stunned;

	if (isKnockBack_ || isStun_ || hp_ <= 0) {
		isAttackMode_ = false;
		isAttacking_ = false;
		attackHitBox_.active = false;
	}
}
