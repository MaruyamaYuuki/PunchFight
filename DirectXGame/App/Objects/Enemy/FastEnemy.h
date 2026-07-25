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
/// ヒット＆アウェイ（一撃離脱）戦法を特徴とする、素早い敵キャラクタークラス
/// EnemyBaseを継承し、攻撃後にプレイヤーから距離を取る専用のAI挙動を持ちます。
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
	/// 離脱（後退）行動の更新処理
	/// 離脱タイマーを進行させ、プレイヤーから遠ざかる方向へ移動を制御します。
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間（秒）</param>
	void UpdateRetreat(float deltaTime);

	/// <summary>
	/// 離脱行動が完了したかどうかを判定します
	/// </summary>
	/// <returns>離脱時間が規定値に達していれば true、それ以外は false</returns>
	bool IsRetreatFinished() const;

	float GetRetreatSpeedMultiplier() const { return retreatSpeedMultiplier_; }

private:
	/// <summary>
	/// 離脱行動を開始する
	/// タイマーをリセットし、離脱中フラグを有効にします。
	/// </summary>
	void StartRetreat();

	/// <summary>
	/// FastEnemy固有の攻撃処理
	/// 攻撃実行後に StartRetreat() を呼び出し、一撃離脱の挙動へ移行します。
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（攻撃ベクトル計算などに使用）</param>
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;

private:

	// --- ヒット＆アラン用 ---
	bool isRetreating_ = false;           // 離脱中フラグ
	float retreatTimer_ = 0.0f;           // 離脱時間タイマー
	float retreatDuration_ = 1.0f;        // 離脱する時間（秒）
	float retreatSpeedMultiplier_ = 1.5f; // 離脱時の速度倍率
};
