#include "NormalEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void NormalEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	RIdleTexture_ = TextureManager::Load("enemies/REnemy.png");
	RAttackTexture_ = TextureManager::Load("enemies/RPunch.png");
	RStunTexture_ = TextureManager::Load("enemies/RStun.png");

	LIdleTexture_ = TextureManager::Load("enemies/LEnemy.png");
	LAttackTexture_ = TextureManager::Load("enemies/LPunch.png");
	LStunTexture_ = TextureManager::Load("enemies/LStun.png");

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
	if (fabs(toPlayer.x) > 0.01f) {
		facingDir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;
	}

	// ===== 攻撃処理 =====
	AttackProcess(playerPos);

	// ===== 攻撃中じゃない＆クールタイム中じゃない =====
	if (dist > ATTACK_RANGE && !isAttackMode_) {
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

void NormalEnemy::MoveTowardPlayer(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {
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

	float finalLen = sqrtf(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
	if (finalLen > 0.001f) {
		toPlayer.x /= finalLen;
		toPlayer.z /= finalLen;
	}

	float moveSpeed = 0.025f;
	worldTransform_.translation_.x += toPlayer.x * moveSpeed;
	worldTransform_.translation_.z += toPlayer.z * moveSpeed;
}

void NormalEnemy::AttackProcess(const Vector3& playerPos) {
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// プレイヤーの向き
	if (fabs(toPlayer.x) > 0.01f)
		facingDir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;

	// ===== 一定距離以内なら攻撃モードON（離れてもOFFにしない） =====
	if (!isAttackMode_ && dist <= ATTACK_RANGE) {
		isAttackMode_ = true;
	}

	// 攻撃モードじゃないなら何もしない
	if (!isAttackMode_) {
		return;
	}

	// ===== クールタイム =====
	if (attackCooldownTimer_ > 0.0f) {
		attackCooldownTimer_ -= deltaTime;
		return; // 攻撃できないのでここで終了
	}

	// ===== 攻撃中処理 =====
	if (isAttacking_) {
		attackTimer_ -= deltaTime;

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

	float offsetX = 0.5f * facingDir_;
	SetAttackHitBox(worldTransform_.translation_ + Vector3{offsetX, 0.1f, 0});
	attackHitBox_.active = true;
}




