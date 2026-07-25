#include "EnemyState.h"
#include "EnemyBase.h"
#include "FastEnemy.h"

// 共通の「被弾・死亡」チェック用ヘルパー
bool CheckCommonTransitions(EnemyBase* enemy) {
	if (enemy->GetHP() <= 0) {
		if (enemy->GetState() != EnemyState::Dead) {
			enemy->ChangeState<EnemyStateDead>();
			return true;
		}
	}
	if (enemy->IsKnockBack()) {
		if (enemy->GetState() != EnemyState::Knockback) {
			enemy->ChangeState<EnemyStateKnockback>();
			return true;
		}
	}
	return false;
}

// =================================================================
// 1. EnemyStateIdle (待機状態)
// =================================================================
void EnemyStateIdle::Update(EnemyBase* enemy) {
	if (CheckCommonTransitions(enemy))
		return;

	// 攻撃範囲内なら攻撃準備へ
	if (enemy->IsAttackMode()) {
		enemy->ChangeState<EnemyStateAttackWait>();
	}
	// 移動中なら歩行へ
	else if (enemy->GetSpeed() > 0.0f) {
		enemy->ChangeState<EnemyStateWalking>();
	}
}

uint32_t EnemyStateIdle::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRIdleTexture() : enemy->GetLIdleTexture(); }

// =================================================================
// 2. EnemyStateWalking (歩行状態)
// =================================================================
void EnemyStateWalking::Update(EnemyBase* enemy) {
	if (CheckCommonTransitions(enemy))
		return;

	if (enemy->IsAttackMode()) {
		enemy->ChangeState<EnemyStateAttackWait>();
	} else if (enemy->GetSpeed() <= 0.0f) {
		enemy->ChangeState<EnemyStateIdle>();
	}
}

uint32_t EnemyStateWalking::GetTexture(EnemyBase* enemy) {
	static const int walkPattern[4] = {0, 1, 2, 3};
	// GetWalkFrame() は EnemyBase に追加した Getter
	int texIndex = walkPattern[enemy->GetWalkFrame()];

	return (enemy->GetFacingDir() > 0) ? enemy->GetRWalkTexture(texIndex) : enemy->GetLWalkTexture(texIndex);
}

// =================================================================
// 3. EnemyStateAttackWait (攻撃予備動作)
// =================================================================
void EnemyStateAttackWait::Update(EnemyBase* enemy) {
	if (CheckCommonTransitions(enemy))
		return;

	// 予備動作が終わり、攻撃が開始されたら Attacking へ遷移
	if (enemy->IsAttacking()) {
		enemy->ChangeState<EnemyStateAttacking>();
	}
}

uint32_t EnemyStateAttackWait::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRWaitTexture() : enemy->GetLWaitTexture(); }

// =================================================================
// 4. EnemyStateAttacking (攻撃実行中)
// =================================================================
void EnemyStateAttacking::Update(EnemyBase* enemy) {
	if (CheckCommonTransitions(enemy))
		return;

	// 攻撃が終了（フラグがfalse）したら Idle へ
	if (!enemy->IsAttacking()) {
		if (enemy->GetState() == EnemyState::Attacking) {
			enemy->ChangeState<EnemyStateIdle>();
		}
	}
}

uint32_t EnemyStateAttacking::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRAttackTexture() : enemy->GetLAttackTexture(); }

// =================================================================
// 5. EnemyStateStunned (スタン中)
// =================================================================
void EnemyStateStunned::Update(EnemyBase* enemy) {
	if (CheckCommonTransitions(enemy))
		return;

	// EnemyBase側の Update で stunTimer_ が 0 になり、
	// isStun_ フラグが降りるのを監視して Idle に戻る
	// (※ EnemyBaseに IsStun() などのゲッターが必要です)
	if (!enemy->IsMovementInterrupted()) {
		enemy->ChangeState<EnemyStateIdle>();
	}
}

uint32_t EnemyStateStunned::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRStunTexture() : enemy->GetLStunTexture(); }

// =================================================================
// 6. EnemyStateKnockback (吹っ飛び中)
// =================================================================
void EnemyStateKnockback::Update(EnemyBase* enemy) {
	// 吹っ飛びフラグが降り、HPがあれば復帰
	if (!enemy->IsKnockBack()) {
		if (enemy->GetHP() > 0) {
			enemy->ChangeState<EnemyStateIdle>();
		} else {
			enemy->ChangeState<EnemyStateDead>();
		}
	}
}

uint32_t EnemyStateKnockback::GetTexture(EnemyBase* enemy) {
	// 元のコードに合わせて StunTexture を流用
	return (enemy->GetFacingDir() > 0) ? enemy->GetRStunTexture() : enemy->GetLStunTexture();
}

// =================================================================
// 7. EnemyStateDead (死亡)
// =================================================================
void EnemyStateDead::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateDead::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRStunTexture() : enemy->GetLStunTexture(); }

// =================================================================
// 8. EnemyStateRetreat (離脱)
// =================================================================
void EnemyStateRetreat::Update(EnemyBase* enemy) {
	if (CheckCommonTransitions(enemy))
		return;

if (auto fast = dynamic_cast<FastEnemy*>(enemy)) {
		float speed = enemy->GetSpeed() * fast->GetRetreatSpeedMultiplier();

		enemy->AddPositionX(-enemy->GetFacingDir() * speed * enemy->GetDeltaTime());

		fast->UpdateRetreat(enemy->GetDeltaTime());

		if (fast->IsRetreatFinished()) {

			enemy->SetAttackMode(false);
			enemy->SetAttacking(false);

			enemy->ChangeState<EnemyStateIdle>();
		}
	}
}

uint32_t EnemyStateRetreat::GetTexture(EnemyBase* enemy) {
	static const int walkPattern[4] = {0, 1, 2, 3};
	// GetWalkFrame() は EnemyBase に追加した Getter
	int texIndex = walkPattern[enemy->GetWalkFrame()];

	return (enemy->GetFacingDir() > 0) ? enemy->GetRWalkTexture(texIndex) : enemy->GetLWalkTexture(texIndex);
}