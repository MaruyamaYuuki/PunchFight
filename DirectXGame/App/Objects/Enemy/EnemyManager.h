#pragma once

#include "EnemyBase.h"
#include "NormalEnemy.h"
#include "PowerEnemy.h"
#include <memory> // unique_ptr
#include <vector>

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
/// 敵の管理
/// </summary>
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
	void AddSpawnToArea(int areaIndex, EnemyType type, const KamataEngine::Vector3& pos);

	/// <summary>
	/// エリア内にいる敵の全滅判定
	/// </summary>
	/// <param name="areaIndex">エリア番号</param>
	/// <returns></returns>
	bool IsAreaCleared(int areaIndex) const;

	/// <summary>
	/// 敵のリストを取得する
	/// </summary>
	/// <returns>敵のポインタを保持するコンテナ（参照）</returns>
	const std::vector<std::unique_ptr<EnemyBase>>& GetEnemies() const { return enemies_; }

private:
	/// <summary>
	/// 敵の出現情報を設定する
	/// </summary>
	/// <param name="type">敵の種類</param>
	/// <param name="pos">出現座標</param>
	void SpawnEnemy(EnemyType type, const KamataEngine::Vector3& pos);

private:
	std::vector<std::unique_ptr<EnemyBase>> enemies_;
	std::vector<EnemyArea> areas_;

	bool isHit_ = false;
};
