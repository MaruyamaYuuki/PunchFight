#pragma once

#include "EnemyBase.h"
#include "NormalEnemy.h"
#include "PowerEnemy.h"
#include "../../../Engine/Particle/SmokeParticleManager.h"
#include <memory> // unique_ptr
#include <vector>
#include <functional>

/// <summary>
/// 敵の種類
/// </summary>
enum class EnemyType { Normal, Power };

/// <summary>
/// 敵の出現トリガー
/// </summary>
struct EnemySpawnTrigger {
	EnemyType type;
	KamataEngine::Vector3 pos;
};

/// <summary>
/// 敵の出現範囲
/// </summary>
struct EnemyArea {
	float triggerX;
	std::vector<EnemySpawnTrigger> spawns;
	bool activated = false;
	bool cleared = false;
};

/// <summary>
/// 敵キャラクター全体の一括管理と制御。
/// 全エネミーオブジェクトの生成・破棄（ライフサイクル）の管理。
/// エネミーとプレイヤー、またはエリア判定との衝突検知の仲介。
/// エリアごとの敵生存数の監視と、エリアクリアフラグの更新。
/// ポリモーフィズムを利用した、基底クラスポインタによる一括更新・描画の実行。
/// </summary>
namespace MyEngine {
    class GameConfigManager;
}
class EnemyManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	void Update(const KamataEngine::Vector3& playerPos);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

	/// <summary>
	/// プレイヤーの後ろに描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="playerPos">プレイヤーの座標</param>
	void BackDraw(KamataEngine::Camera& camera, const KamataEngine::Vector3& playerPos);

	/// <summary>
	/// プレイヤーの前に描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="playerPos">プレイヤーの座標</param>
	void FrontDraw(KamataEngine::Camera& camera, const KamataEngine::Vector3& playerPos);

	/// <summary>
	/// 敵出現エリアの追加
	/// </summary>
	/// <param name="triggerX">エリアの開始座標</param>
	void AddArea(float triggerX);

	/// <summary>
	/// エリア内への敵の出現
	/// </summary>
	/// <param name="areaIndex">エリア番号</param>
	/// <param name="type">敵の種類</param>
	/// <param name="pos">出現座標</param>
	void AddSpawnToArea(int32_t areaIndex, EnemyType type, const KamataEngine::Vector3& pos);

	/// <summary>
	/// エリア内にいる敵の全滅判定
	/// </summary>
	/// <param name="areaIndex">判定対象のエリア番号</param>
	/// <returns>エリア内の全ての敵（SpawnTrigger）が出現し、かつ撃破されていれば true</returns>
	bool IsAreaCleared(int32_t areaIndex) const;

	/// <summary>
	/// 敵のリストを取得する
	/// </summary>
	/// <returns>敵のポインタを保持するコンテナ（参照）</returns>
	const std::vector<std::unique_ptr<EnemyBase>>& GetEnemies() const { return enemies_; }

private:
	/// <summary>
	/// 敵キャラクターの動的生成
	/// </summary>
	/// <param name="type">生成する敵の種類（Normal/Power）</param>
	/// <param name="pos">生成時のワールド座標</param>
	void SpawnEnemy(EnemyType type, const KamataEngine::Vector3& pos);


	/// <summary>
	/// Zソートされた敵リストの取得
	/// </summary>
	/// <param name="playerPos">プレイヤーの現在座標（前後判定の基準）</param>
	/// <param name="backSide">プレイヤーより奥の敵を取得する場合は true、手前の場合は false</param>
	/// <returns>指定された条件でソートされた敵ポインタのリスト</returns>
	std::vector<EnemyBase*> GetEnemiesSortedByZ(const KamataEngine::Vector3& playerPos, bool backSide);

private:
	std::vector<std::unique_ptr<EnemyBase>> enemies_;
	std::unique_ptr<MyEngine::SmokeParticleManager> smokeManager_;
	std::vector<EnemyArea> areas_;
	MyEngine::GameConfigManager* cfg_ = nullptr;

	bool isHit_ = false;
};
