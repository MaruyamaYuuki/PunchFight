#include "PlayerCombat.h"
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void PlayerCombat::Initialize() {
	cfg = GameConfigManager::GetInstance();

	normalPower_ = cfg->getInt("Player.Attack.kNormalAttackPower");
	normalDuration_ = cfg->getInt("Player.Attack.kNormalAttackDuration");
	normalCooldown_ = cfg->getInt("Player.Attack.kNormalAttackCoolDown");
	normalHitBox_.size = cfg->getVector3("Player.Attack.kNormalAttackHitBoxSize");

	specialPower_ = cfg->getInt("Player.Attack.kSPAttackPower");
	specialDuration_ = cfg->getFloat("Player.Attack.kSPAttackDuration");
	specialCooldown_ = cfg->getFloat("Player.Attack.kSPAttackCoolDown");
	specialSpeed_ = cfg->getFloat("Player.Attack.kSPAttackMoveSpeed");
	specialHitBox_.size = cfg->getVector3("Player.Attack.kSPAttackHitBoxSize");
}

void PlayerCombat::Update(const Vector3& playerPos, float facingDir) {

    justStartedNormal_ = false;
	justStartedSpecial_ = false;

	// ===== 通常攻撃 =====
	if (isNormalAttacking_) {
		normalTimer_--;
		if (normalTimer_ <= 0) {
			isNormalAttacking_ = false;
			normalHitBox_.active = false;
			normalCooldownTimer_ = normalCooldown_;
		} else {
			float hitboxOffsetX = 0.5f * facingDir; // プレイヤーが右向きなら+0.8、左向きなら-0.8
			normalHitBox_.pos = playerPos + Vector3{hitboxOffsetX, 0.1f, 0.0f};
		}
	}

	// クールタイム中
	else if (!canNormalAttack_) {
		normalCooldownTimer_--;
		if (normalCooldownTimer_ <= 0) {
			canNormalAttack_ = true;
		}
	}

	// ===== 強攻撃 =====

	// ---------------------
	// 気弾の更新
	// ---------------------
	if (isSpecialAttacking_) {

		specialTimer_ -= deltaTime_;

		// 発射時の向きで移動
		specialPos_.x += specialSpeed_ * specialDir_;

		// ヒットボックス追従
		specialHitBox_.pos = specialPos_ + Vector3{specialDir_ * 0.3f, 0.0f, 0.0f};

		// 気弾が消える
		if (specialTimer_ <= 0.0f) {
			isSpecialAttacking_ = false;
			specialHitBox_.active = false;
		}
	}

	// ---------------------
	// クールタイム進行（気弾が生きていても進む）
	// ---------------------
	if (!canSpecialAttack_) {
		specialCooldownTimer_ -= deltaTime_;

		if (specialCooldownTimer_ <= 0.0f) {
			canSpecialAttack_ = true;
			specialCooldownTimer_ = 0.0f;
		}
	}
}

void PlayerCombat::StartNormalAttack(KamataEngine::Vector3 playerPos, float facingDir) {
	if (!canNormalAttack_ || isNormalAttacking_)
		return;

	isNormalAttacking_ = true;
	canNormalAttack_ = false;
	justStartedNormal_ = true;
	normalTimer_ = normalDuration_;

	// パンチテクスチャ切り替え（右左交互）
	attackFromRight_ = !attackFromRight_;

	// ヒットボックスはプレイヤーの向きに依存
	float hitboxOffsetX = 0.5f * facingDir; // プレイヤーが右向きなら+0.8、左向きなら-0.8
	normalHitBox_.active = true;
	normalHitBox_.pos = playerPos+ Vector3{hitboxOffsetX, 0.1f, 0.0f};
	normalHitBox_.size = cfg->getVector3("Player.Attack.kNormalAttackHitBoxSize");
}

void PlayerCombat::StartSpecialAttack(KamataEngine::Vector3 playerPos, float facingDir) {
	if (!canSpecialAttack_ || isSpecialAttacking_)
		return;

	isSpecialAttacking_ = true;
	canSpecialAttack_ = false;
	justStartedSpecial_ = true;

	// --- クールタイムを即スタート ---
    specialCooldownTimer_ = specialCooldown_;

	specialTimer_ = specialDuration_;

	// 発射時の向きを固定
	specialDir_ = static_cast<float>(facingDir);

	// --- 気弾モデルの初期位置 ---
	specialPos_ = playerPos + Vector3{specialDir_ * 0.5f, 0.2f, 0.0f};

	// --- ヒットボックス初期化 ---
	specialHitBox_.active = true;
	specialHitBox_.pos = specialPos_+ Vector3{specialDir_ * 0.3f, 0.0f, 0.0f};
	specialHitBox_.size = cfg->getVector3("Player.Attack.kSPAttackHitBoxSize");

}