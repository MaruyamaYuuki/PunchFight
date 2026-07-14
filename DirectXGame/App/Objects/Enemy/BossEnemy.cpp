#include "BossEnemy.h"
#include "../../../Engine/Utility/GameConfigManager.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void BossEnemy::Initialize(const EnemyData& data) {
	EnemyBase::Initialize(data);

	cfg_ = GameConfigManager::GetInstance();

	// PowerEnemyと同じ設定を利用
	tackleChargeTime_ = cfg_->getFloat("Enemy.Types.Power.kTackleChargeTime");
	tackleMoveTime_ = cfg_->getFloat("Enemy.Types.Power.kTackleMoveTime");
	tackleSpeed_ = cfg_->getFloat("Enemy.Types.Power.kTackleSpeed");

	SetRIdleTexture(TextureManager::Load("enemies/powerEnemy/RPower.png"));
	SetRWaitTexture(TextureManager::Load("enemies/powerEnemy/RHeadbutt1.png"));
	SetRAttackTexture(TextureManager::Load("enemies/powerEnemy/RHeadbutt2.png"));
	SetRStunTexture(TextureManager::Load("enemies/powerEnemy/RStun.png"));
	SetRWalkTexture(0, TextureManager::Load("enemies/powerEnemy/RWalk1.png"));
	SetRWalkTexture(1, TextureManager::Load("enemies/powerEnemy/RWalk2.png"));
	SetRWalkTexture(2, TextureManager::Load("enemies/powerEnemy/RWalk3.png"));
	SetRWalkTexture(3, TextureManager::Load("enemies/powerEnemy/RWalk4.png"));

	SetLIdleTexture(TextureManager::Load("enemies/powerEnemy/LPower.png"));
	SetLWaitTexture(TextureManager::Load("enemies/powerEnemy/LHeadbutt1.png"));
	SetLAttackTexture(TextureManager::Load("enemies/powerEnemy/LHeadbutt2.png"));
	SetLStunTexture(TextureManager::Load("enemies/powerEnemy/RStun.png"));
	SetLWalkTexture(0, TextureManager::Load("enemies/powerEnemy/LWalk1.png"));
	SetLWalkTexture(1, TextureManager::Load("enemies/powerEnemy/LWalk2.png"));
	SetLWalkTexture(2, TextureManager::Load("enemies/powerEnemy/LWalk3.png"));
	SetLWalkTexture(3, TextureManager::Load("enemies/powerEnemy/LWalk4.png"));

	ResetAttackCooldown();
}

void BossEnemy::Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {
	// ノックバック・スタン・死亡時
	if (IsMovementInterrupted()) {
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// プレイヤーとの距離
	KamataEngine::Vector3 toPlayer = playerPos - GetPosition();
	float dist = Length(toPlayer);

	// 攻撃中でないときはプレイヤーの方向を向く
	if (!IsAttackMode() && !IsAttacking()) {
		if (fabs(toPlayer.x) > 0.01f) {
			SetFacingDir((toPlayer.x > 0.0f) ? 1.0f : -1.0f);
		}
	}

	// 攻撃処理
	AttackProcess(playerPos);

	// 攻撃モードでなければ追尾
	if (dist > GetAttackRange() && !IsAttackMode()) {
		MoveTowardPlayer(playerPos, allEnemies);
	}

	// 衝撃波攻撃の終了処理
	if (shockWaveActive_) {
		shockWaveTimer_ -= GetDeltaTime();

		if (shockWaveTimer_ <= 0) {
			shockWaveActive_ = false;

			SetAttackHitBoxActive(false);

			SetAttackMode(false);
			SetAttacking(false);

			ResetAttackCooldown();
		}
	}

	// 状態遷移
	if (IsAttacking()) {
		if (GetState() != EnemyState::Attacking) {
			ChangeState<EnemyStateAttacking>();
		}
	} else if (IsAttackMode()) {
		if (GetState() != EnemyState::AttackWait) {
			ChangeState<EnemyStateAttackWait>();
		}
	} else if (dist > GetAttackRange()) {
		if (GetState() != EnemyState::Walking) {
			ChangeState<EnemyStateWalking>();
		}
	} else {
		if (GetState() != EnemyState::Idle) {
			ChangeState<EnemyStateIdle>();
		}
	}

	EnemyBase::Update(playerPos, allEnemies);
}


void BossEnemy::AttackProcess(const KamataEngine::Vector3& playerPos) {
	// 攻撃モードに入っていないなら距離チェック
	if (!IsAttackMode()) {
		KamataEngine::Vector3 toPlayer = playerPos - GetPosition();
		float dist = Length(toPlayer);

		if (dist <= GetAttackRange()) {
			EnterAttackMode(playerPos);
		} else {
			return;
		}
	}

if (useJumpAttack_) {

		JumpAttack();

	} else if (useTripleTackle_) {

		TripleTackleAttack();

	} else {

		DoNormalAttack(playerPos);
	}
}

void BossEnemy::TripleTackleAttack() {
	// =========================
	// 1. 溜め開始
	// =========================
	if (!isTackleCharging_ && !isTackling_) {
		// 3回終わっていたら攻撃終了
		if (tackleCount_ >= maxTackleCount_) {
			SetAttackMode(false);
			SetAttacking(false);
			SetAttackHitBoxActive(false);
			ResetAttackCooldown();
			return;
		}

		isTackleCharging_ = true;
		tackleChargeTimer_ = tackleChargeTime_;
		return;
	}

	// =========================
	// 2. 溜め中
	// =========================
	if (isTackleCharging_) {
		tackleChargeTimer_ -= GetDeltaTime();

		if (tackleChargeTimer_ <= 0.0f) {
			isTackleCharging_ = false;
			isTackling_ = true;

			SetAttacking(true);

			tackleMoveTimer_ = tackleMoveTime_;

			// 自分全体を攻撃判定に
			SetAttackHitBoxActive(true);
			SetAttackHitBoxPos(GetPosition());
			SetAttackHitBoxSize(GetHitBox().size);
		}
		return;
	}

	// =========================
	// 3. タックル中
	// =========================
	if (isTackling_) {
		tackleMoveTimer_ -= GetDeltaTime();

		// 記録しておいた向きへ突進
		AddPositionX(GetFacingDir() * tackleSpeed_);

		// 攻撃判定追従
		SetAttackHitBoxPos(GetPosition());

		// タックル終了
		if (tackleMoveTimer_ <= 0.0f) {
			isTackling_ = false;
			SetAttacking(false);
			SetAttackHitBoxActive(false);

			// 1回分完了
			tackleCount_++;

			// まだ残っている場合は、
			// 次のフレームで再度プレイヤー方向を向き直して溜め開始
			if (tackleCount_ < maxTackleCount_) {
				// AttackProcess()で再度プレイヤー位置を見て
				// EnterAttackModeせずそのまま継続する。
				// 向きだけ更新したいのでAttackDirXを現在のFacingDirに合わせる。
				SetAttackDirX(GetFacingDir());
			}
		}
	}
}

void BossEnemy::EnterAttackMode(const KamataEngine::Vector3& playerPos) {
	// 攻撃モードをオンにする
	SetAttackMode(true);

	// プレイヤーの方向を計算して振り向く
	KamataEngine::Vector3 toPlayer = playerPos - GetPosition();
	if (fabs(toPlayer.x) > 0.01f) {
		SetFacingDir((toPlayer.x > 0.0f) ? 1.0f : -1.0f);
	}
	SetAttackDirX(GetFacingDir());

	// ボス特有の攻撃パターンのリセットと決定
	// （ここでは例として、常に3連タックルを行うように設定しています）
	useTripleTackle_ = true;
	tackleCount_ = 0;
	isTackleCharging_ = false;
	isTackling_ = false;
}

void BossEnemy::JumpAttack() {
	// 溜め
	if (!isJumpCharging_ && !isJumping_) {
		isJumpCharging_ = true;
		jumpChargeTimer_ = jumpChargeTime_;
		return;
	}

	if (isJumpCharging_) {
		jumpChargeTimer_ -= GetDeltaTime();

		if (jumpChargeTimer_ <= 0.0f) {
			isJumpCharging_ = false;

			isJumping_ = true;

			jumpVelocityY_ = jumpPower_;
		}

		return;
	}

	// 空中
	if (isJumping_) {
		jumpVelocityY_ -= jumpGravity_ * GetDeltaTime();

		AddPositionY(jumpVelocityY_ * GetDeltaTime());

		// 着地
		if (GetPosition().y <= groundY_) {
			Vector3 pos = GetPosition();
			pos.y = groundY_;
			SetPosition(pos.x, pos.y, pos.z);

			isJumping_ = false;

			ShockWaveAttack();
		}
	}
}

void BossEnemy::ShockWaveAttack() {
	shockWaveActive_ = true;

	shockWaveTimer_ = shockWaveTime_;

	SetAttackHitBoxActive(true);

	SetAttackHitBoxPos(GetPosition());

	SetAttackHitBoxSize({2.5f, 0.6f, 2.5f});
}
