#include "NormalEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void NormalEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	RIdleTexture_ = TextureManager::Load("enemies/normalEnemy/RNormal.png");
	RAttackTexture_ = TextureManager::Load("enemies/normalEnemy/RPunch.png");
	RWaitTexture_ = TextureManager::Load("enemies/normalEnemy/RNormal.png");
	RStunTexture_ = TextureManager::Load("enemies/normalEnemy/RStun.png");
	RWalkTexture_[0] = TextureManager::Load("enemies/normalEnemy/RWalk1.png");
	RWalkTexture_[1] = TextureManager::Load("enemies/normalEnemy/RWalk2.png");
	RWalkTexture_[2] = TextureManager::Load("enemies/normalEnemy/RWalk3.png");
	RWalkTexture_[3] = TextureManager::Load("enemies/normalEnemy/RWalk2.png");

	LIdleTexture_ = TextureManager::Load("enemies/normalEnemy/LNormal.png");
	LAttackTexture_ = TextureManager::Load("enemies/normalEnemy/LPunch.png");
	LWaitTexture_ = TextureManager::Load("enemies/normalEnemy/LNormal.png");
	LStunTexture_ = TextureManager::Load("enemies/normalEnemy/LStun.png");
	LWalkTexture_[0] = TextureManager::Load("enemies/normalEnemy/LWalk1.png");
	LWalkTexture_[1] = TextureManager::Load("enemies/normalEnemy/LWalk2.png");
	LWalkTexture_[2] = TextureManager::Load("enemies/normalEnemy/LWalk3.png");
	LWalkTexture_[3] = TextureManager::Load("enemies/normalEnemy/LWalk2.png");

	ResetAttackCooldown();
}

void NormalEnemy::Update(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {

    // ===== ノックバック中・スタン中・ノックアウト中は何もしない =====
	if (IsMovementInterrupted()) {
		UpdateBasicState();
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// ===== プレイヤーとの距離計算 =====
	Vector3 toPlayer = playerPos - GetPosition();
	float dist = std::sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// プレイヤー方向（左右）を決める
	if (!IsAttackMode() && !IsAttacking()) {
		if (fabs(toPlayer.x) > 0.01f) {
			float dir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;
			SetFacingDir(dir_);
		}
	}

	// ===== 攻撃処理 =====
	AttackProcess(playerPos);

	// ===== 攻撃中じゃない＆クールタイム中じゃない =====
	if (dist > GetAttackRange() && !IsAttackMode()) {
		MoveTowardPlayer(playerPos, allEnemies);
	} 

    // 移動・攻撃などの状態判定
	if (IsAttacking())
		state_ = EnemyState::Attacking;
	else if (IsAttackMode())
		state_ = EnemyState::AttackWait;
	else if (GetSpeed() > 0.0f)
		state_ = EnemyState::Walking;
	else
		state_ = EnemyState::Idle;

	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	GetWorldTransform().UpdateMatrix();
}

void NormalEnemy::AttackProcess(const Vector3& playerPos) { 
	DoNormalAttack(playerPos); 
}