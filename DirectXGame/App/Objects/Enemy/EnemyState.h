#pragma once
#include <cstdint>

// 前方宣言：EnemyBaseポインタを引数で使うために必要
class EnemyBase;

/// <summary>
/// 敵状態の基底クラス (インターフェース)
/// </summary>
class BaseEnemyState {
public:
	virtual ~BaseEnemyState() = default;

	// 状態が始まった時に一度だけ呼ばれる
	virtual void Enter(EnemyBase* enemy) { (void)enemy; }

	// この状態の時の更新ロジック
	virtual void Update(EnemyBase* enemy) = 0;

	// この状態の時に表示すべきテクスチャを取得
	virtual uint32_t GetTexture(EnemyBase* enemy) = 0;

	// 次の状態へ移る直前に一度だけ呼ばれる
	virtual void Exit(EnemyBase* enemy) { (void)enemy; }
};

// =================================================================
// 各具体的な状態クラスの定義
// =================================================================

/// <summary> 待機状態 </summary>
class EnemyStateIdle : public BaseEnemyState {
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> 歩行状態 </summary>
class EnemyStateWalking : public BaseEnemyState {
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> 攻撃予備動作状態 </summary>
class EnemyStateAttackWait : public BaseEnemyState {
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> 攻撃実行状態 </summary>
class EnemyStateAttacking : public BaseEnemyState {
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> スタン(怯み)状態 </summary>
class EnemyStateStunned : public BaseEnemyState {
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> 吹っ飛び状態 </summary>
class EnemyStateKnockback : public BaseEnemyState {
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> 死亡状態 </summary>
class EnemyStateDead : public BaseEnemyState {
public:
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};

/// <summary> 離脱状態（ヒット＆アラン） </summary>
class EnemyStateRetreat : public BaseEnemyState {
public:
public:
	void Update(EnemyBase* enemy) override;
	uint32_t GetTexture(EnemyBase* enemy) override;
};