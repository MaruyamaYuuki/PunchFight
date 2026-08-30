#include "PlayerCombat.h"
#include "../../../Engine/Utility/GameConfigManager.h"
#include "../../../Engine/Utility/TimeManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void PlayerCombat::Initialize() {

	normalPower_ = GameConfigManager::GetInstance()->getInt("Player.Attack.kNormalAttackPower");
	normalDuration_ = GameConfigManager::GetInstance()->getInt("Player.Attack.kNormalAttackDuration");
	normalCooldown_ = GameConfigManager::GetInstance()->getInt("Player.Attack.kNormalAttackCoolDown");
	normalHitBox_.size = GameConfigManager::GetInstance()->getVector3("Player.Attack.kNormalAttackHitBoxSize");

	specialPower_ = GameConfigManager::GetInstance()->getInt("Player.Attack.kSPAttackPower");
	specialDuration_ = GameConfigManager::GetInstance()->getFloat("Player.Attack.kSPAttackDuration");
	specialCooldown_ = GameConfigManager::GetInstance()->getFloat("Player.Attack.kSPAttackCoolDown");
	specialSpeed_ = GameConfigManager::GetInstance()->getFloat("Player.Attack.kSPAttackMoveSpeed");
	specialHitBox_.size = GameConfigManager::GetInstance()->getVector3("Player.Attack.kSPAttackHitBoxSize");
}

void PlayerCombat::Update(const Vector3& playerPos, float facingDir) {

    justStartedNormal_ = false;
	justStartedSpecial_ = false;

// ==========================================
	// ★追加: 一定時間攻撃しなかったらコンボリセット
	// ==========================================
	if (!isNormalAttacking_ && comboCount_ > 0) {
		comboResetTimer_ -= deltaTime_;
		if (comboResetTimer_ <= 0.0f) {
			comboCount_ = 0;
		}
	}

	// ===== 通常攻撃の更新 =====
	if (isNormalAttacking_) {
		normalTimer_--;

		// ヒットボックス追従
		float offsetX = 0.5f * facingDir;
		normalHitBox_.pos = playerPos + KamataEngine::Vector3{offsetX, 0.1f, 0.0f};

		if (normalTimer_ <= 0) {
			// 1発分の攻撃終了
			isNormalAttacking_ = false;
			normalHitBox_.active = false;

			// ==========================================
			// 3連撃モード中の処理
			// ==========================================
			if (isAutoCombo_) {
				autoComboStep_++;
				if (autoComboStep_ <= 3) {
					// 3連撃中：クールタイムを挟まず、即座に次のパンチを出す
					isNormalAttacking_ = true;
					justStartedNormal_ = true; // SEやエフェクトを再度発生させる
					normalTimer_ = normalDuration_;
					attackFromRight_ = !attackFromRight_;
					normalHitBox_.active = true;
				} else {
					// 3発終わったら自動連撃終了 → クールタイムへ
					isAutoCombo_ = false;
					canNormalAttack_ = false;
					normalCooldownTimer_ = normalCooldown_;
				}
			} else {
				// 通常の1発攻撃終了時 → クールタイムへ
				canNormalAttack_ = false;
				normalCooldownTimer_ = normalCooldown_;
			}
		}
	} else if (!canNormalAttack_) {
		// クールタイム中
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

		specialTimer_ -= TimeManager::GetInstance()->GetDeltaTime();

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
		specialCooldownTimer_ -= TimeManager::GetInstance()->GetDeltaTime();

		if (specialCooldownTimer_ <= 0.0f) {
			canSpecialAttack_ = true;
			specialCooldownTimer_ = 0.0f;
		}
	}
}

void PlayerCombat::StartNormalAttack(KamataEngine::Vector3 playerPos, float facingDir) {
	if (!canNormalAttack_ || isNormalAttacking_)
		return;

	// コンボのカウントアップ
	comboCount_++;
	comboResetTimer_ = comboResetTimeMax_; // タイマーリセット

	// 4回目なら3連撃モード突入
	if (comboCount_ >= 4) {
		isAutoCombo_ = true;
		autoComboStep_ = 1;
		comboCount_ = 0; // 次のためにコンボ数をリセットしておく
	}

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
	normalHitBox_.size = GameConfigManager::GetInstance()->getVector3("Player.Attack.kNormalAttackHitBoxSize");
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
	specialHitBox_.size = GameConfigManager::GetInstance()->getVector3("Player.Attack.kSPAttackHitBoxSize");

}