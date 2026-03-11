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
		UpdateBasicState();
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// 2. 離脱（ヒット＆アラン）処理
	if (isRetreating_) {
		retreatTimer_ -= GetDeltaTime();

		// プレイヤーから離れる方向に移動
		float retreatDir = (playerPos.x > GetPosition().x) ? -1.0f : 1.0f;
		AddPositionX(retreatDir * GetSpeed() * retreatSpeedMultiplier_);

		SetFacingDir(retreatDir);
		SetState(EnemyState::Walking);

		if (retreatTimer_ <= 0.0f) {
			isRetreating_ = false;
			ResetAttackCooldown();
		}

		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// 3. 通常の追従・攻撃処理
	Vector3 toPlayer = playerPos - GetPosition();
	float dist = Length(toPlayer);

	// 攻撃前の状態を記録しておく
	bool wasAttackMode = IsAttackMode();

	// 攻撃範囲内かつクールタイムが終わっていれば攻撃開始
	if (dist <= GetAttackRange() && !IsAttackMode() && GetAttackCoolDownTimer() <= 0.0f) {
		SetAttackMode(true);
	}

	if (IsAttackMode()) {
		AttackProcess(playerPos);
	} else {
		// プレイヤーを追いかける
		MoveTowardPlayer(playerPos, allEnemies);
		SetState(EnemyState::Walking);
	}

	// ★修正のキモ：攻撃モードが「ON」から「OFF」に切り替わった瞬間を検知
	if (wasAttackMode && !IsAttackMode()) {
		StartRetreat();
	}

	// プレイヤーの方向を向く（攻撃中・離脱中でない場合のみ）
	if (!IsAttackMode() && !isRetreating_ && std::abs(toPlayer.x) > 0.01f) {
		SetFacingDir((toPlayer.x > 0) ? 1.0f : -1.0f);
	}

	EnemyBase::Update(playerPos, allEnemies);
}

void FastEnemy::StartRetreat() {
	isRetreating_ = true;
	retreatTimer_ = retreatDuration_;
}
