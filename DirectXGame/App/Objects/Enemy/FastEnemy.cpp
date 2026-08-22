#include "FastEnemy.h"
#include "../../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;
using MyEngine::GameConfigManager;

void FastEnemy::Initialize(const EnemyData& data) {
	EnemyBase::Initialize(data);

    // 待機時間を設定
	waitDuration_ = GameConfigManager::GetInstance()->getFloat("Enemy.Types.Fast.kWaitDuration");

	SetRIdleTexture(TextureManager::Load("enemies/FastEnemy/RFast.png"));
	SetRAttackTexture(TextureManager::Load("enemies/FastEnemy/RPunch.png"));
	SetRWaitTexture(TextureManager::Load("enemies/FastEnemy/RFast.png"));
	SetRStunTexture(TextureManager::Load("enemies/FastEnemy/RStun.png"));
	SetRWalkTexture(0, TextureManager::Load("enemies/FastEnemy/RWalk1.png"));
	SetRWalkTexture(1, TextureManager::Load("enemies/FastEnemy/RWalk2.png"));
	SetRWalkTexture(2, TextureManager::Load("enemies/FastEnemy/RWalk3.png"));
	SetRWalkTexture(3, TextureManager::Load("enemies/FastEnemy/RWalk2.png"));

	SetLIdleTexture(TextureManager::Load("enemies/FastEnemy/LFast.png"));
	SetLAttackTexture(TextureManager::Load("enemies/FastEnemy/LPunch.png"));
	SetLWaitTexture(TextureManager::Load("enemies/FastEnemy/LFast.png"));
	SetLStunTexture(TextureManager::Load("enemies/FastEnemy/LStun.png"));
	SetLWalkTexture(0, TextureManager::Load("enemies/FastEnemy/LWalk1.png"));
	SetLWalkTexture(1, TextureManager::Load("enemies/FastEnemy/LWalk2.png"));
	SetLWalkTexture(2, TextureManager::Load("enemies/FastEnemy/LWalk3.png"));
	SetLWalkTexture(3, TextureManager::Load("enemies/FastEnemy/LWalk2.png"));

    // 攻撃前待機時間を1秒に設定し、タイマーをリセット
	preAttackDuration_ = 1.0f;
	preAttackTimer_ = preAttackDuration_;
}

void FastEnemy::Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {

	if (IsMovementInterrupted()) {
		// 被弾してスタンやノックバックした場合はコンボと待機をリセット
		comboCount_ = 0;
		isWaiting_ = false;
		isPreAttackWaiting_ = false; // ★追加: 攻撃前待機もリセット
		preAttackTimer_ = preAttackDuration_;
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

	// ==========================================
	// プレイヤーとの距離とベクトルを先に計算する
	// ==========================================
	KamataEngine::Vector3 toPlayer = playerPos - GetPosition();
	float dist = KamataEngine::MathUtility::Length(toPlayer);

	// ==========================================
	// プレイヤー方向（左右）を決める
	// ==========================================
	// コンボ中や攻撃前待機中は振り向かない
	if (!IsAttackMode() && !IsAttacking() && comboCount_ == 0 && !isPreAttackWaiting_) {
		if (std::abs(toPlayer.x) > 0.01f) {
			float dir = (toPlayer.x > 0.0f) ? 1.0f : -1.0f;
			SetFacingDir(dir);
		}
	}

	// 毎フレーム確実にクールタイムを減算する
	UpdateAttackCooldown(GetDeltaTime());

	// 待機終了判定
	if (isWaiting_ && IsWaitFinished()) {
		isWaiting_ = false;
		comboCount_ = 0; // コンボ回数をリセットして再び追従可能にする
	}

	// 3連撃後の待機中の場合
	if (isWaiting_) {
		UpdateWait(GetDeltaTime());
		EnemyBase::Update(playerPos, allEnemies);
		return;
	}

    // ==========================================
	// 攻撃の開始判定と移動の順番を整理
	// ==========================================
	KamataEngine::Vector3 attackTargetPos = playerPos;
	if (comboCount_ > 0) {
		attackTargetPos = GetPosition();
		attackTargetPos.x += GetFacingDir() * 0.1f; // 目の前のダミー座標
	}

	if (IsAttackMode() || comboCount_ > 0) {
		// 既に攻撃中、またはコンボ途中ならそのまま継続
		isPreAttackWaiting_ = false;
		AttackProcess(attackTargetPos);

	} else if (isPreAttackWaiting_) {
		// 一度「攻撃前待機」に入ったら、距離の変動に関わらずタイマーを進める（リセットされない）
		preAttackTimer_ -= GetDeltaTime();
		if (preAttackTimer_ <= 0.0f) {
			isPreAttackWaiting_ = false;

			// ★修正: 1秒待っている間にプレイヤーが射程外に離れてしまった場合、
			// 基底クラス側で攻撃がキャンセルされてしまうのを防ぐため、ダミー座標を渡す
			KamataEngine::Vector3 dummyPos = GetPosition();
			dummyPos.x += GetFacingDir() * 0.1f;
			AttackProcess(dummyPos);
		}

	} else if (dist <= GetAttackRange() && GetAttackCoolDownTimer() <= 0.0f) {
		// 攻撃範囲内に入った瞬間、1秒の待機フェーズを開始する
		isPreAttackWaiting_ = true;
		preAttackTimer_ = preAttackDuration_;

	} else {
		// 攻撃範囲外なら追従移動
		isPreAttackWaiting_ = false;
		preAttackTimer_ = preAttackDuration_;
		MoveTowardPlayer(playerPos, allEnemies);
	}

	// ==========================================
	// 移動・攻撃などの状態判定
	// ==========================================
	if (IsAttacking()) {
		if (GetState() != EnemyState::Attacking)
			ChangeState<EnemyStateAttacking>();
	} else if (IsAttackMode()) {
		if (GetState() != EnemyState::AttackWait)
			ChangeState<EnemyStateAttackWait>();
	} else if (GetSpeed() > 0.0f && dist > GetAttackRange() && !isPreAttackWaiting_) {
		// 攻撃前待機中ではない時だけ Walking
		if (GetState() != EnemyState::Walking)
			ChangeState<EnemyStateWalking>();
	} else {
		// 攻撃前待機中を含め、それ以外は Idle で立ち止まる
		if (GetState() != EnemyState::Idle)
			ChangeState<EnemyStateIdle>();
	}

	EnemyBase::Update(playerPos, allEnemies);
	GetWorldTransform().UpdateMatrix();
}

void FastEnemy::UpdateWait(float deltaTime) { waitTimer_ -= deltaTime; }

bool FastEnemy::IsWaitFinished() const { return waitTimer_ <= 0.0f; }

void FastEnemy::StartWait() {
	isWaiting_ = true;
	waitTimer_ = waitDuration_;

	SetAttackMode(false);
	SetAttacking(false);
}

void FastEnemy::AttackProcess(const KamataEngine::Vector3& playerPos) {
	// 攻撃前の状態を保存
	bool wasAttacking = IsAttacking();

	// 通常攻撃処理
	DoNormalAttack(playerPos);

	// 攻撃終了を検知した瞬間
	if (wasAttacking && !IsAttacking()) {
		comboCount_++;

		if (comboCount_ >= maxComboCount_) {
			// 3回攻撃したら待機状態へ
			StartWait();

			// AttackWaitテクスチャではなくIdleテクスチャを再生するように変更
			ChangeState<EnemyStateIdle>();
		} else {
			// まだコンボ中の場合、親クラスの UpdateAttackCooldown に大きな値を渡して
			// クールタイムを強制的に 0 にし、即座に次の攻撃を出せるようにする
			UpdateAttackCooldown(100.0f);
		}
	}
}