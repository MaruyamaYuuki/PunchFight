#pragma once
#include "../../../Engine/Math/Collider.h"
#include "KamataEngine.h"

namespace MyEngine {
    class GameConfigManager;
}
class PlayerCombat {
public:
	void Initialize();

	void Update(const KamataEngine::Vector3& playerPos, float facingDir);

	void StartNormalAttack(KamataEngine::Vector3 playerPos, float facingDir);
	void StartSpecialAttack(KamataEngine::Vector3 playerPos, float facingDir);

	const HitBox& GetNormalHitBox() const { return normalHitBox_; }
	const HitBox& GetSpecialHitBox() const { return specialHitBox_; }

	int GetNormalPower() const { return normalPower_; }
	int GetSpecialPower() const { return specialPower_; }

	bool IsNormalAttacking() const { return isNormalAttacking_; }
	bool IsSpecialAttacking() const { return isSpecialAttacking_; }

	bool DidStartNormalAttack() const { return justStartedNormal_; }
	bool DidStartSpecialAttack() const { return justStartedSpecial_; }

	float GetSpecialDir() const { return specialDir_; }

	KamataEngine::Vector3 GetSpecialPos() const { return specialPos_; }

	bool CanSpecial() const { return canSpecialAttack_; }

	bool GetAttackForRight() const { return attackFromRight_; }

	float GetSpecialCooldownTimer() const { return specialCooldownTimer_; }

	float GetSpecialCooldown() const { return specialCooldown_; }

private:
	MyEngine::GameConfigManager* cfg = nullptr;

	// ===== 通常攻撃 =====
	bool isNormalAttacking_ = false;
	bool canNormalAttack_ = true;
	bool justStartedNormal_ = false;
	int normalTimer_ = 0;
	int normalCooldownTimer_ = 0;

	int normalPower_;
	int normalDuration_;
	int normalCooldown_;

	bool attackFromRight_ = true;

	HitBox normalHitBox_;

	// ===== 強攻撃 =====
	bool isSpecialAttacking_ = false;
	bool canSpecialAttack_ = true;
	bool justStartedSpecial_ = false;

	float specialTimer_ = 0.0f;
	float specialCooldownTimer_ = 0.0f;

	float specialDuration_;
	float specialCooldown_;
	float specialSpeed_;
	int specialPower_;

	float specialDir_ = 1.0f;

	KamataEngine::Vector3 specialPos_;
	HitBox specialHitBox_;

	const float deltaTime_ = 1.0f / 60.0f;
};