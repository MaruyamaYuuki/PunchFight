#include "NormalEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void NormalEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	RIdleTexture_ = TextureManager::Load("enemies/REnemy.png");
	RAttackTexture_ = TextureManager::Load("enemies/RPunch.png");
	RStunTexture_ = TextureManager::Load("enemies/RStun.png");
	RWalkTexture_[0] = TextureManager::Load("enemies/REnemy.png");
	RWalkTexture_[1] = TextureManager::Load("enemies/REnemy.png");
	RWalkTexture_[2] = TextureManager::Load("enemies/REnemy.png");
	RWalkTexture_[3] = TextureManager::Load("enemies/REnemy.png");

	LIdleTexture_ = TextureManager::Load("enemies/LEnemy.png");
	LAttackTexture_ = TextureManager::Load("enemies/LPunch.png");
	LStunTexture_ = TextureManager::Load("enemies/LStun.png");
	LWalkTexture_[0] = TextureManager::Load("enemies/LEnemy.png");
	LWalkTexture_[1] = TextureManager::Load("enemies/LEnemy.png");
	LWalkTexture_[2] = TextureManager::Load("enemies/LEnemy.png");
	LWalkTexture_[3] = TextureManager::Load("enemies/LEnemy.png");

	attackCooldownTimer_ = attackCooldown_;
}

void NormalEnemy::Update(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {

    // ===== ノックバック中・スタン中・ノックアウト中は何もしない =====
	if (isKnockBack_ || isStun_ || hp_ <= 0) {
		EnemyBase::Update(playerPos, allEnemies);
		isAttackMode_ = false;
		isAttacking_ = false;
		attackHitBox_.active = false;
		worldTransform_.UpdateMatrix();

		// 状態をDeadまたはKnockbackに設定
		if (isKnockBack_)
			state_ = EnemyState::Knockback;
		else if (hp_ <= 0)
			state_ = EnemyState::Dead;
		else if (isStun_)
			state_ = EnemyState::Stunned;

		return;
	}

	// ===== プレイヤーとの距離計算 =====
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = std::sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// プレイヤー方向（左右）を決める
	if (!isAttackMode_ && !isAttacking_) {
		if (fabs(toPlayer.x) > 0.01f) {
			facingDir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;
		}
	}

	// ===== 攻撃処理 =====
	AttackProcess(playerPos);

	// ===== 攻撃中じゃない＆クールタイム中じゃない =====
	if (dist > ATTACK_RANGE_ && !isAttackMode_) {
		MoveTowardPlayer(playerPos, allEnemies);
	} 

    // 移動・攻撃などの状態判定
    if (isAttacking_)
		state_ = EnemyState::Attacking;
	else if (speed_ > 0.0f)
		state_ = EnemyState::Walking;
	else
		state_ = EnemyState::Idle;

	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	worldTransform_.UpdateMatrix();
}

void NormalEnemy::AttackProcess(const Vector3& playerPos) { 
	DoNormalAttack(playerPos); 
}