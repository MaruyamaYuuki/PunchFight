#include "FastEnemy.h"
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void FastEnemy::Initialize(const EnemyData& data) {
	EnemyBase::Initialize(data);

	cfg_ = GameConfigManager::GetInstance();
	retreatDuration_ = cfg_->getFloat("Enemy.Types.Fast.kRetreatDuration");
	retreatSpeedMultiplier_ = cfg_->getFloat("Enemy.Types.Fast.kRetreatSpeedMultiplier");

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
	// 1. 中断状態のチェック
	if (IsMovementInterrupted()) {
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// 2. 攻撃終了の検知用
	bool wasAttacking = IsAttacking();

	// 3. 通常の追従・攻撃ロジック
	if (GetState() == EnemyState::Idle || GetState() == EnemyState::Walking) {
		Vector3 toPlayer = playerPos - GetPosition();
		float dist = Length(toPlayer);

		if (dist <= GetAttackRange() && GetAttackCoolDownTimer() <= 0.0f) {
			SetAttackMode(true);
		}

		if (IsAttackMode()) {
			AttackProcess(playerPos);
		} else {
			MoveTowardPlayer(playerPos, allEnemies);
			// 状態が変わったことをステートクラスに伝える
			if (GetState() != EnemyState::Walking)
				ChangeState<EnemyStateWalking>();
		}
	}

	// ★重要：攻撃が終わった瞬間に「離脱ステート」へ切り替える
	if (wasAttacking && !IsAttacking()) {
		ChangeState<EnemyStateRetreat>();
	}

	// 親クラスの更新
	EnemyBase::Update(playerPos, allEnemies);
}
void FastEnemy::StartRetreat() {
	isRetreating_ = true;
	retreatTimer_ = retreatDuration_;
}
