#include "EnemyState.h"
#include "EnemyBase.h"

// =================================================================
// 1. EnemyStateIdle (待機状態)
// =================================================================
void EnemyStateIdle::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateIdle::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRIdleTexture() : enemy->GetLIdleTexture(); }

// =================================================================
// 2. EnemyStateWalking (歩行状態)
// =================================================================
void EnemyStateWalking::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateWalking::GetTexture(EnemyBase* enemy) {
	static const int walkPattern[4] = {0, 1, 2, 3};
	// GetWalkFrame() は EnemyBase に追加した Getter
	int texIndex = walkPattern[enemy->GetWalkFrame()];

	return (enemy->GetFacingDir() > 0) ? enemy->GetRWalkTexture(texIndex) : enemy->GetLWalkTexture(texIndex);
}

// =================================================================
// 3. EnemyStateAttackWait (攻撃予備動作)
// =================================================================
void EnemyStateAttackWait::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateAttackWait::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRWaitTexture() : enemy->GetLWaitTexture(); }

// =================================================================
// 4. EnemyStateAttacking (攻撃実行中)
// =================================================================
void EnemyStateAttacking::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateAttacking::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRAttackTexture() : enemy->GetLAttackTexture(); }

// =================================================================
// 5. EnemyStateStunned (スタン中)
// =================================================================
void EnemyStateStunned::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateStunned::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRStunTexture() : enemy->GetLStunTexture(); }

// =================================================================
// 6. EnemyStateKnockback (吹っ飛び中)
// =================================================================
void EnemyStateKnockback::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateKnockback::GetTexture(EnemyBase* enemy) {
	// 元のコードに合わせて StunTexture を流用
	return (enemy->GetFacingDir() > 0) ? enemy->GetRStunTexture() : enemy->GetLStunTexture();
}

// =================================================================
// 7. EnemyStateDead (死亡)
// =================================================================
void EnemyStateDead::Update(EnemyBase* enemy) { (void)enemy; }

uint32_t EnemyStateDead::GetTexture(EnemyBase* enemy) { return (enemy->GetFacingDir() > 0) ? enemy->GetRStunTexture() : enemy->GetLStunTexture(); }