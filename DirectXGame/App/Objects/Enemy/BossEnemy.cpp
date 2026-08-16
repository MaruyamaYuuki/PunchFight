#include "BossEnemy.h"
#include "../../../Engine/Utility/GameConfigManager.h"
#include "../../../Engine//Utility/TimeManager.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void BossEnemy::Initialize(const EnemyData& data) {
	EnemyBase::Initialize(data);

	// bossEnemyと同じ設定を利用
	tackleChargeTime_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleChargeTime");
	tackleMoveTime_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleMoveTime");
	tackleSpeed_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleSpeed");

	shockWaveModel_.reset(KamataEngine::Model::CreateFromOBJ("shockWave", true));

	// トランスフォームの初期化
	shockWaveTransform_.Initialize();

	SetRIdleTexture(TextureManager::Load("enemies/bossEnemy/RBoss.png"));
	SetRWaitTexture(TextureManager::Load("enemies/bossEnemy/RHeadbutt1.png"));
	SetRAttackTexture(TextureManager::Load("enemies/bossEnemy/RHeadbutt2.png"));
	SetRStunTexture(TextureManager::Load("enemies/bossEnemy/RStun.png"));
	SetRWalkTexture(0, TextureManager::Load("enemies/bossEnemy/RWalk1.png"));
	SetRWalkTexture(1, TextureManager::Load("enemies/bossEnemy/RWalk2.png"));
	SetRWalkTexture(2, TextureManager::Load("enemies/bossEnemy/RWalk3.png"));
	SetRWalkTexture(3, TextureManager::Load("enemies/bossEnemy/RWalk4.png"));
	RTackleTexture_ = TextureManager::Load("enemies/bossEnemy/RTackle.png");
	RTackleWaitTexture_ = TextureManager::Load("enemies/bossEnemy/RTackleWait.png");
	RJumpAttackTexture_ = TextureManager::Load("enemies/bossEnemy/RJump.png");

	SetLIdleTexture(TextureManager::Load("enemies/bossEnemy/LBoss.png"));
	SetLWaitTexture(TextureManager::Load("enemies/bossEnemy/LHeadbutt1.png"));
	SetLAttackTexture(TextureManager::Load("enemies/bossEnemy/LHeadbutt2.png"));
	SetLStunTexture(TextureManager::Load("enemies/bossEnemy/RStun.png"));
	SetLWalkTexture(0, TextureManager::Load("enemies/bossEnemy/LWalk1.png"));
	SetLWalkTexture(1, TextureManager::Load("enemies/bossEnemy/LWalk2.png"));
	SetLWalkTexture(2, TextureManager::Load("enemies/bossEnemy/LWalk3.png"));
	SetLWalkTexture(3, TextureManager::Load("enemies/bossEnemy/LWalk4.png"));
	LTackleTexture_ = TextureManager::Load("enemies/bossEnemy/LTackle.png");
	LTackleWaitTexture_ = TextureManager::Load("enemies/bossEnemy/LTackleWait.png");
	LJumpAttackTexture_ = TextureManager::Load("enemies/bossEnemy/LJump.png");

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
			attackType_ = BossAttackType::Normal;
			SetAttackHitBoxActive(false);
			SetAttackMode(false);
			SetAttacking(false);
			ResetAttackCooldown();
		} else {
			// 進行度を計算（0.0f = 発生直後, 1.0f = 消滅寸前）
			float progress = 1.0f - (shockWaveTimer_ / shockWaveTime_);

			// 進行度に合わせて現在の半径を 0 から最大値まで拡大する
			currentShockWaveRadius_ = maxShockWaveRadius_ * progress;

			// 衝撃波がアクティブな間、トランスフォームを更新して可視化の準備をする
			shockWaveTransform_.translation_ = GetPosition();
			float adjustMultiplier = 0.5f; // ★この数値を変更して見た目を当たり判定に合わせる

			// 固定の GetShockWaveRadius() ではなく、変動する currentShockWaveRadius_ を使う
			float visualScale = currentShockWaveRadius_ * 2.0f * adjustMultiplier;
			shockWaveTransform_.scale_ = {visualScale, 0.5f, visualScale};
			shockWaveTransform_.UpdateMatrix();
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
	shockWaveTransform_.UpdateMatrix();
}

void BossEnemy::Draw(KamataEngine::Camera& camera) {
	// 親クラスの描画（ボス本体、通常ヒットボックス、パーティクル等）
	EnemyBase::Draw(camera);

	// 衝撃波がアクティブな場合のみ描画
	if (shockWaveActive_ && shockWaveModel_) {
		shockWaveModel_->Draw(shockWaveTransform_, camera);
	}
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
			attackType_ = BossAttackType::Normal;
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

			tackleCount_++;

			if (tackleCount_ < maxTackleCount_) {

				float nextDir = -GetFacingDir();

				SetFacingDir(nextDir);
				SetAttackDirX(nextDir);

				isTackleCharging_ = false;
			} else {
				attackType_ = BossAttackType::Normal;
			}
		}
	}
}

void BossEnemy::EnterAttackMode(const KamataEngine::Vector3& playerPos) {
	// 攻撃モードをオンにする
	SetAttackMode(true);

	groundY_ = GetPosition().y; 

	// プレイヤーの方向を計算して振り向く
	KamataEngine::Vector3 toPlayer = playerPos - GetPosition();
	if (fabs(toPlayer.x) > 0.01f) {
		SetFacingDir((toPlayer.x > 0.0f) ? 1.0f : -1.0f);
	}
	SetAttackDirX(GetFacingDir());

	float r = static_cast<float>(rand()) / RAND_MAX;

if (r < 0.3f) {

		attackType_ = BossAttackType::Jump;

		useJumpAttack_ = true;
		useTripleTackle_ = false;

		isJumpCharging_ = false;
		isJumping_ = false;
		shockWaveActive_ = false;
	} else if (r < 0.7f) {

		attackType_ = BossAttackType::TripleTackle;

		useJumpAttack_ = false;
		useTripleTackle_ = true;

		tackleCount_ = 0;
		isTackleCharging_ = false;
		isTackling_ = false;
	} else {

		attackType_ = BossAttackType::Normal;

		useJumpAttack_ = false;
		useTripleTackle_ = false;
	}
}

void BossEnemy::JumpAttack() {
	if (shockWaveActive_) {
		return;
	}

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

			SetAttacking(true);

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
	// 着地した瞬間の1フレームだけ呼ばれるため、発生処理のみを記述する
	shockWaveActive_ = true;
	shockWaveTimer_ = shockWaveTime_; // 設定した持続時間
	currentShockWaveRadius_ = 0.0f;   // 発生時は半径を0にリセット
	SetHasDealtDamage(false);         // ダメージフラグをリセット
}

uint32_t BossEnemy::GetRWaitTexture() const {
	switch (attackType_) {
	case BossAttackType::TripleTackle:
		return RTackleWaitTexture_;

	case BossAttackType::Jump:
		return RJumpAttackTexture_;

	default:
		return EnemyBase::GetRWaitTexture();
	}
}

uint32_t BossEnemy::GetRAttackTexture() const {
	switch (attackType_) {
	case BossAttackType::TripleTackle:
		return RTackleTexture_;

	case BossAttackType::Jump:
		return RJumpAttackTexture_;

	default:
		return EnemyBase::GetRAttackTexture();
	}
}

uint32_t BossEnemy::GetLWaitTexture() const {
	switch (attackType_) {
	case BossAttackType::TripleTackle:
		return LTackleWaitTexture_;

	case BossAttackType::Jump:
		return LJumpAttackTexture_;

	default:
		return EnemyBase::GetLWaitTexture();
	}
}

uint32_t BossEnemy::GetLAttackTexture() const {
	switch (attackType_) {
	case BossAttackType::TripleTackle:
		return LTackleTexture_;

	case BossAttackType::Jump:
		return LJumpAttackTexture_;

	default:
		return EnemyBase::GetLAttackTexture();
	}
}
