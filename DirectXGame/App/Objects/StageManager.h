#pragma once
#include "KamataEngine.h"
#include "Stage.h"
#include <deque>

/// <summary>
/// ステージ管理
/// </summary>
class StageManager {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	StageManager() = default;

	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~StageManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="stageNumber">ステージの番号</param>
	/// <param name="repeatCount">生成するステージモデルの個数</param>
	void Initialize(int stageNumber, int repeatCount);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="cameraX">カメラのX座標</param>
	void Update(float cameraX);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

private:
	/// <summary>
	/// オブジェクトの読み込み
	/// </summary>
	void UpdateLoadedStages();

private:
	std::unordered_map<int, std::unique_ptr<Stage>> activeStages_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* startModel_ = nullptr;
	KamataEngine::Model* midModel_ = nullptr;
	KamataEngine::Model* endModel_ = nullptr;

	float stageWidth = 20.0f; // 区間の幅
	int currentIndex_ = 0;    // 現在位置の区間インデックス
	int stageCount_ = 0;      // ステージ全体の区間数
};
