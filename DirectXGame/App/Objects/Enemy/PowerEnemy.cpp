#include "PowerEnemy.h"
#include "../../../Engine/Utility/GameConfigManager.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void PowerEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	tackleProbability_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleProbability");
	tackleChargeTime_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleChargeTime");
	tackleMoveTime_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleMoveTime");
	tackleSpeed_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Power.kTackleSpeed");


	SetRIdleTexture(TextureManager::Load("enemies/powerEnemy/RPower.png"));
	SetRWaitTexture(TextureManager::Load("enemies/powerEnemy/RHeadbutt1.png"));
	SetRAttackTexture(TextureManager::Load("enemies/powerEnemy/RHeadbutt2.png"));
	RTackleTexture_ = TextureManager::Load("enemies/powerEnemy/RTackle.png");
	RTackleWaitTexture_ = TextureManager::Load("enemies/powerEnemy/RTackleWait.png");
	SetRStunTexture(TextureManager::Load("enemies/powerEnemy/RStun.png"));
	SetRWalkTexture(0, TextureManager::Load("enemies/powerEnemy/RWalk1.png"));
	SetRWalkTexture(1, TextureManager::Load("enemies/powerEnemy/RWalk2.png"));
	SetRWalkTexture(2, TextureManager::Load("enemies/powerEnemy/RWalk3.png"));
	SetRWalkTexture(3, TextureManager::Load("enemies/powerEnemy/RWalk4.png"));

	SetLIdleTexture(TextureManager::Load("enemies/powerEnemy/LPower.png"));
	SetLWaitTexture(TextureManager::Load("enemies/powerEnemy/LHeadbutt1.png"));
	SetLAttackTexture(TextureManager::Load("enemies/powerEnemy/LHeadbutt2.png"));
	LTackleTexture_ = TextureManager::Load("enemies/powerEnemy/LTackle.png");
	LTackleWaitTexture_ = TextureManager::Load("enemies/powerEnemy/LTackleWait.png");
	SetLStunTexture(TextureManager::Load("enemies/powerEnemy/RStun.png"));
	SetLWalkTexture(0, TextureManager::Load("enemies/powerEnemy/LWalk1.png"));
	SetLWalkTexture(1, TextureManager::Load("enemies/powerEnemy/LWalk2.png"));
	SetLWalkTexture(2, TextureManager::Load("enemies/powerEnemy/LWalk3.png"));
	SetLWalkTexture(3, TextureManager::Load("enemies/powerEnemy/LWalk4.png"));

	SetAttackSEDataHandle(GetAudio()->LoadWave("audio/SE/headbuttSE.wav"));

	SetStunMultiplier(0.3f);

	ResetAttackCooldown();
}

void PowerEnemy::Update(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {

	// ===== ノックバック中・スタン中・ノックアウト中は何もしない =====
	if (IsMovementInterrupted()) {
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// ===== プレイヤーとの距離計算 =====
	Vector3 toPlayer = playerPos - GetPosition();
	float dist = std::sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// プレイヤー方向（左右）を決める
	if (!IsAttackMode() && !IsAttacking()) {
    	if (fabs(toPlayer.x) > 0.01f) {
    		float dir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;
			SetFacingDir(dir_);
    	}
	}

	// ===== 攻撃処理 =====
	AttackProcess(playerPos);

	// ===== 攻撃中じゃない＆クールタイム中じゃない =====
	if (dist > GetAttackRange() && !IsAttackMode()) {
		MoveTowardPlayer(playerPos, allEnemies);
	}

	// 移動・攻撃などの状態判定
	if (IsAttacking()) {
		if (GetState() != EnemyState::Attacking)
			ChangeState<EnemyStateAttacking>();
	} else if (IsAttackMode()) {
		if (GetState() != EnemyState::AttackWait)
			ChangeState<EnemyStateAttackWait>();
	} else if (GetSpeed() > 0.0f && dist > GetAttackRange()) {
		if (GetState() != EnemyState::Walking)
			ChangeState<EnemyStateWalking>();
	} else {
		if (GetState() != EnemyState::Idle)
			ChangeState<EnemyStateIdle>();
	}

	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	GetWorldTransform().UpdateMatrix();
}

void PowerEnemy::TackleAttack() {
	// ===== 溜め開始 =====
	if (!isTackleCharging_ && !isTackling_) {
		isTackleCharging_ = true;
		tackleChargeTimer_ = tackleChargeTime_;
		return;
	}

	// ===== 溜め中 =====
	if (isTackleCharging_) {
		tackleChargeTimer_ -= GetDeltaTime();

		if (tackleChargeTimer_ <= 0.0f) {
			isTackleCharging_ = false;
			isTackling_ = true;
			SetAttacking(true);

			tackleMoveTimer_ = tackleMoveTime_;

			// 自分自身を攻撃判定に
			SetAttackHitBoxActive(true);
			SetAttackHitBoxPos(GetPosition());
			SetAttackHitBoxSize(GetHitBox().size);
		}
		return;
	}

	// ===== タックル中 =====
	if (isTackling_) {
		tackleMoveTimer_ -= GetDeltaTime();

		// 固定した向きで突進
		float dir_ = GetFacingDir();
		AddPositionX(dir_ * tackleSpeed_);

		SetAttackHitBoxPos(GetPosition());

		if (tackleMoveTimer_ <= 0.0f) {
			isTackling_ = false;
			SetAttacking(false);
			SetAttackMode(false);

			SetAttackHitBoxActive(false);
			ResetAttackCooldown();
		}
	}
}

void PowerEnemy::EnterAttackMode(const Vector3& playerPos) { 
	SetAttackMode(true);

	// 向き固定
	Vector3 toPlayer = playerPos - GetPosition();
	SetAttackDirX((toPlayer.x >= 0) ? 1.0f : -1.0f);
	float dir_ = GetAttackDirX();
	SetFacingDir(dir_);

	// 攻撃タイプ抽選
	float r = static_cast<float>(rand()) / RAND_MAX;
	if (r < tackleProbability_) {
		attackType_ = AttackType::Tackle;
		useTackle_ = true;
	} else {
		attackType_ = AttackType::Normal;
		useTackle_ = false;
	}
}

void PowerEnemy::AttackProcess(const KamataEngine::Vector3& playerPos) {
	// 攻撃モードに入ってないならチェック
	if (!IsAttackMode()) {
		Vector3 toPlayer = playerPos - GetPosition();
		float dist = Length(toPlayer);

		if (dist <= GetAttackRange()) {
			EnterAttackMode(playerPos); // ★ここで一度だけ決定
		} else {
			return;
		}
	}

	if (useTackle_) {
		TackleAttack();
	} else {
		DoNormalAttack(playerPos);
	}
}

void PowerEnemy::UpdateTextures() {
	// 共通処理
	EnemyBase::UpdateTextures();
}
