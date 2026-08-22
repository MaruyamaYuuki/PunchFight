#pragma once
#include "EnemyBase.h"

// 残像一つ分のデータ
struct GhostEffect {
	KamataEngine::Vector3 pos;
	float facingDir;
	float alpha;
	uint32_t textureHandle;
};

/// <summary>
/// スピードタイプのエネミー。
/// 接近して3連撃を行った後、一定時間待機する挙動を持ちます。
/// </summary>
class FastEnemy : public EnemyBase {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	FastEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data">エネミーデータ</param>
	void Initialize(const EnemyData& data) override;

	/// 更新処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（追従対象）</param>
	/// <param name="allEnemies">全エネミーのリスト（他の敵との重なり回避計算に使用）</param>
	virtual void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies);

    /// <summary>
	/// 待機行動の更新処理
	/// </summary>
	void UpdateWait(float deltaTime);

	/// <summary>
	/// 待機行動が完了したかどうかを判定します
	/// </summary>
	bool IsWaitFinished() const;

private:
	/// <summary>
	/// 待機を開始する
	/// </summary>
	void StartWait();

	/// <summary>
	/// FastEnemy固有の攻撃処理
	/// 3連続で攻撃を行い、終了後に待機状態へ移行します。
	/// </summary>
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

private:
	// --- 3連撃＆待機用パラメータ ---
	bool isWaiting_ = false;    // 待機中フラグ
	float waitTimer_ = 0.0f;    // 待機時間タイマー
	float waitDuration_ = 2.0f; // 3連撃後の待機時間（秒）

	int comboCount_ = 0;          // 現在の攻撃回数
	const int maxComboCount_ = 3; // 連撃の最大数

	// 攻撃前待機用パラメータ
	bool isPreAttackWaiting_ = false; // 接敵後、攻撃前の待機フラグ
	float preAttackTimer_ = 0.0f;     // 攻撃前待機タイマー
	float preAttackDuration_ = 1.0f;  // 攻撃前の待機時間（秒）
};
