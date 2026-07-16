#include "EnemyBase.h"
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;
using MyEngine::SmokeParticleManager;
using MyEngine::DustParticleManager;

void EnemyBase::Initialize(const EnemyData& data) {

    model_.reset(Model::CreateFromOBJ(data.modelPath, true));
	speed_ = data.speed;
	hp_ = data.hp;
	attackPower_ = data.attackPower;


	modelEHitBox_.reset(Model::CreateFromOBJ("boxFrame", true));
	modelAHitBox_.reset(Model::CreateFromOBJ("boxFrame", true));

	worldTransform_.Initialize();
	worldTransform_.rotation_.x = 0.75f;
	worldTransformEHitBox_.Initialize();
	worldTransformAHitBox_.Initialize();

	smokeManager_ = std::make_unique<SmokeParticleManager>();
	smokeManager_->Initialize();

	dustManager_ = std::make_unique<DustParticleManager>();
	dustManager_->Initialize();

	worldTransform_.rotation_.x = GameConfigManager::GetInstance()->getFloat("Global.kPlaneModelRotateX");
	walkFrameInterval_ = GameConfigManager::GetInstance()->getInt("Enemy.Default.kWalkFrameInterval");
	gravity_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kGravityAcceleration");
	knockbackDuration_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kKnockbackDuration");
	attackDuration_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kAttackDuration");
	attackCooldown_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kAttackCooldown");
	attackRange_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kAttackRange");
	stunDuration_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kStunDuration");
	stunShakeAmplitude_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kStunShakeAmplitude");
	stunShakeSpeed_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kStunShakeSpeed");
	smokeSpawnInterval_ = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kSmokeSpawnInterval");
	smokeSize_ = GameConfigManager::GetInstance()->getVector3("Enemy.Default.kSmokeSize");

	attackSEDataHandle_ = Audio::GetInstance()->LoadWave("audio/SE/punchSE.wav");
	hitSEDataHandle_ = Audio::GetInstance()->LoadWave("audio/SE/hitSE.wav");
	blownSEDataHandle_ = Audio::GetInstance()->LoadWave("audio/SE/blownSE.wav");

	ChangeState<EnemyStateIdle>();
}

void EnemyBase::Update(const Vector3&, const std::vector<std::unique_ptr<EnemyBase>>&) {
	if (enemyState_) {
		enemyState_->Update(this);
	}

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
		float drag = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kKnockbackDragFactor");

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
	if (!isStun_) {
		stunGauge_ -= 20.0f * deltaTime_;

		if (stunGauge_ < 0.0f) {
			stunGauge_ = 0.0f;
		}
	}

	if (isStun_ && hp_ > 0) {
		worldTransform_.translation_.x += stunBackVelocity_ * deltaTime_;

		stunBackVelocity_ *= 0.9f;

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

	if (isAirborne_) {
		airVelocityY_ -= gravity_ * deltaTime_;

		worldTransform_.translation_.y += airVelocityY_ * deltaTime_;

		// 着地
		if (worldTransform_.translation_.y <= groundY_) {
			worldTransform_.translation_.y = groundY_;

			isAirborne_ = false;
			airVelocityY_ = 0.0f;

			// 着地スタン
			isStun_ = true;
			stunTimer_ = stunDuration_;
			stunShakeTime_ = 0.0f;

			originalPosition_ = worldTransform_.translation_;

			ChangeState<EnemyStateStunned>();
		} else {
			ChangeState<EnemyStateIdle>();
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
		blownSEVoiceHandle_ = Audio::GetInstance()->PlayWave(blownSEDataHandle_, false, 0.5f);

		hp_ = 0;
		isKnockBack_ = true;
		ChangeState<EnemyStateKnockback>();
		knockbackTime_ = 0.0f;

		// スマブラ風 初速
		float basePower = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kKnockbackBasePower"); // 吹っ飛び強さ
		float upwardBoost = GameConfigManager::GetInstance()->getFloat("Enemy.Default.kKnockbackUpwardBoost"); // 上方向の初速

		knockbackVelocity_ = {attackDir.x * basePower, upwardBoost, 0.0f};
	} else {
		hitSEVoiceHandle_ = Audio::GetInstance()->PlayWave(hitSEDataHandle_, false, 0.5f);

		if (!isStun_) {
			float stunDamage = 25.0f;

			stunGauge_ += stunDamage * stunMultiplier_;
		}


		if (stunGauge_ >= stunGaugeMax_) {
			stunGauge_ = 0.0f;

			isStun_ = true;

			ChangeState<EnemyStateStunned>();
			stunTimer_ = stunDuration_;
			stunShakeTime_ = 0.0f;
			originalPosition_ = worldTransform_.translation_;
			stunBackVelocity_ = -facingDir_ * 3.0f;
		}
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

void EnemyBase::ChangeState(std::unique_ptr<BaseEnemyState> newState) {
	// 1. 現在の状態があれば、終了処理（Exit）を呼ぶ
	if (enemyState_) {
		enemyState_->Exit(this);
	}

	// 2. 新しい状態に差し替え（古い状態は自動的にメモリ解放されます）
	enemyState_ = std::move(newState);

	// 3. 新しい状態の開始処理（Enter）を呼ぶ
	if (enemyState_) {
		enemyState_->Enter(this);
	}
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

    attackSEVoiceHandle_ = Audio::GetInstance()->PlayWave(attackSEDataHandle_, false, 0.5f);

	float offsetX = 0.5f * facingDir_;
	SetAttackHitBox(worldTransform_.translation_ + Vector3{offsetX, 0.1f, 0});
	attackHitBox_.active = true;
}

void EnemyBase::UpdateTextures() {
	if (enemyState_) {
		textureHandle_ = enemyState_->GetTexture(this);
	}
}

bool EnemyBase::IsMovementInterrupted() const {
	// 基本ルール：ノックバック中、スタン中、死亡時は動きを止める
	return isKnockBack_ || isStun_ || hp_ <= 0;
}

void EnemyBase::Launch(float power) {
	isAirborne_ = true;
	airVelocityY_ = power;

	stunOnLanding_ = true;

	ChangeState<EnemyStateKnockback>();
}
