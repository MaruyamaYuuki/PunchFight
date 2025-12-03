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
	void Initialize(int32_t stageNumber, int32_t repeatCount);

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
	std::unordered_map<int32_t, std::unique_ptr<Stage>> activeStages_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* startModel_ = nullptr;
	KamataEngine::Model* midModel_ = nullptr;
	KamataEngine::Model* endModel_ = nullptr;

	float stageWidth = 20.0f; // 区間の幅
	int32_t currentIndex_ = 0;    // 現在位置の区間インデックス
	int32_t stageCount_ = 0;      // ステージ全体の区間数
};
