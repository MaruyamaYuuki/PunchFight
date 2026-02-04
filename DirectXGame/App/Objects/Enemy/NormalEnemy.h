#pragma once
#include "EnemyBase.h"

/// <summary>
/// 基本的な挙動を持つ標準的な敵キャラクターの実装。
/// プレイヤーとの距離に基づいたシンプルな接近・待機AIのルーチン実行。
/// 標準的な攻撃予備動作および攻撃判定の発生制御。
/// 数を多く出すことを前提とした、軽量な更新処理の維持。
/// EnemyBase の仮想関数をオーバーライドし、平均的な移動速度や攻撃力を設定する。
/// </summary>
class NormalEnemy : public EnemyBase {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	NormalEnemy() = default;

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

protected:
	/// <summary>
	/// プレイヤーとの距離に基づいた通常攻撃の実行管理
	/// </summary>
	/// <param name="playerPos">プレイヤーの現在座標</param>
	void AttackProcess(const KamataEngine::Vector3& playerPos) override;
};
