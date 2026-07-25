#include "FastEnemy.h"
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void FastEnemy::Initialize(const EnemyData& data) {
	EnemyBase::Initialize(data);

	retreatDuration_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Fast.kRetreatDuration");
	retreatSpeedMultiplier_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Fast.kRetreatSpeedMultiplier");

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

void FastEnemy::Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {

if (IsMovementInterrupted()) {
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	if (GetState() == EnemyState::Retreat) {
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	if (IsAttackMode()) {
		AttackProcess(playerPos);
	} else {
		MoveTowardPlayer(playerPos, allEnemies);

		Vector3 toPlayer = playerPos - GetPosition();

		if (Length(toPlayer) <= GetAttackRange() && GetAttackCoolDownTimer() <= 0.0f) {
			SetAttackMode(true);
		}
	}

	EnemyBase::Update(playerPos, allEnemies);
}

void FastEnemy::UpdateRetreat(float deltaTime) { retreatTimer_ -= deltaTime; }

bool FastEnemy::IsRetreatFinished() const { return retreatTimer_ <= 0.0f; }

void FastEnemy::StartRetreat() {
	isRetreating_ = true;
	retreatTimer_ = retreatDuration_;

	// 通常攻撃状態を終了
	SetAttackMode(false);
	SetAttacking(false);
}

void FastEnemy::AttackProcess(const KamataEngine::Vector3& playerPos) {
	// 攻撃前の状態を保存
	bool wasAttacking = IsAttacking();

	// 通常攻撃処理
	DoNormalAttack(playerPos);

	// 攻撃終了を検知
	if (wasAttacking && !IsAttacking()) {
		StartRetreat();
		ChangeState<EnemyStateRetreat>();
	}
}