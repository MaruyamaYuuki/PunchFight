#pragma once
#include "KamataEngine.h"
#include "Stage.h"
#include <deque>

/// <summary>
/// ステージ管理
/// </summary>
class StageManager {
public:
	StageManager() = default;

	~StageManager() = default;

	void Initialize(int stageNumber, int repeatCount);

	void Update(float cameraX);

	void Draw(KamataEngine::Camera& camera);

private:
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
