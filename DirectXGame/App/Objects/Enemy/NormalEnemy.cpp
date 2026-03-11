#include "NormalEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void NormalEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	SetRIdleTexture(TextureManager::Load("enemies/normalEnemy/RNormal.png"));
	SetRAttackTexture(TextureManager::Load("enemies/normalEnemy/RPunch.png"));
	SetRWaitTexture(TextureManager::Load("enemies/normalEnemy/RNormal.png"));
	SetRStunTexture(TextureManager::Load("enemies/normalEnemy/RStun.png"));
	SetRWalkTexture(0, TextureManager::Load("enemies/normalEnemy/RWalk1.png"));
	SetRWalkTexture(1, TextureManager::Load("enemies/normalEnemy/RWalk2.png"));
	SetRWalkTexture(2, TextureManager::Load("enemies/normalEnemy/RWalk3.png"));
	SetRWalkTexture(3, TextureManager::Load("enemies/normalEnemy/RWalk2.png"));

	SetLIdleTexture(TextureManager::Load("enemies/normalEnemy/LNormal.png"));
	SetLAttackTexture(TextureManager::Load("enemies/normalEnemy/LPunch.png"));
	SetLWaitTexture(TextureManager::Load("enemies/normalEnemy/LNormal.png"));
	SetLStunTexture(TextureManager::Load("enemies/normalEnemy/LStun.png"));
	SetLWalkTexture(0, TextureManager::Load("enemies/normalEnemy/LWalk1.png"));
	SetLWalkTexture(1, TextureManager::Load("enemies/normalEnemy/LWalk2.png"));
	SetLWalkTexture(2, TextureManager::Load("enemies/normalEnemy/LWalk3.png"));
	SetLWalkTexture(3, TextureManager::Load("enemies/normalEnemy/LWalk2.png"));

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
	if (IsAttacking()) {
		if (GetState() != EnemyState::Attacking)
			ChangeState<EnemyStateAttacking>();
	} else if (IsAttackMode()) {
		if (GetState() != EnemyState::AttackWait)
			ChangeState<EnemyStateAttackWait>();
	} else if (GetSpeed() > 0.0f && dist > GetAttackRange()) {
		// 移動中かつプレイヤーが射程外ならWalking
		if (GetState() != EnemyState::Walking)
			ChangeState<EnemyStateWalking>();
	} else {
		// それ以外は Idle
		if (GetState() != EnemyState::Idle)
			ChangeState<EnemyStateIdle>();
	}

	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	GetWorldTransform().UpdateMatrix();
}

void NormalEnemy::AttackProcess(const Vector3& playerPos) { 
	DoNormalAttack(playerPos); 
}