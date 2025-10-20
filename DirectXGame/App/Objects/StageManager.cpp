#include "StageManager.h"
#include <cmath>

using namespace KamataEngine;

void StageManager::Initialize(int stageNumber, int repeatCount) {
	stageCount_ = repeatCount;
	currentIndex_ = INT32_MIN;

	// モデルパス設定
	switch (stageNumber) {
	case 1:
		startModel_ = KamataEngine::Model::CreateFromOBJ("load", true);
		midModel_ = KamataEngine::Model::CreateFromOBJ("load", true);
		endModel_ = KamataEngine::Model::CreateFromOBJ("load", true);
		break;

	case 2:
		startModel_ = KamataEngine::Model::CreateFromOBJ("stage_cave_start.obj", true);
		midModel_ = KamataEngine::Model::CreateFromOBJ("stage_cave.obj", true);
		endModel_ = KamataEngine::Model::CreateFromOBJ("stage_cave_end.obj", true);
		break;
	}
}


void StageManager::Update(float cameraX) {
	// スクロールに対して余裕を持たせる
	float preloadOffset = stageWidth * 0.3f; // 0.3倍の余裕を持って先読み

	int newIndex = static_cast<int>(std::floor((cameraX + preloadOffset) / stageWidth));

	if (newIndex != currentIndex_) {
		currentIndex_ = newIndex;
		UpdateLoadedStages();
	}

	for (auto& pair : activeStages_) {
		pair.second->Update();
	}
}

void StageManager::UpdateLoadedStages() {
	std::vector<int> requiredIndices;

	for (int offset = -1; offset <= 1; ++offset) {
		int index = currentIndex_ + offset;
		if (index < 0 || index >= stageCount_)
			continue;
		requiredIndices.push_back(index);
	}

	// 不要削除
	for (auto it = activeStages_.begin(); it != activeStages_.end();) {
		if (std::find(requiredIndices.begin(), requiredIndices.end(), it->first) == requiredIndices.end()) {
			it = activeStages_.erase(it);
		} else {
			++it;
		}
	}

	// 必要生成
	for (int index : requiredIndices) {
		if (activeStages_.find(index) == activeStages_.end()) {

			auto stage = std::make_unique<Stage>();

			// モデル切り替え
			KamataEngine::Model* modelToUse = nullptr;
			if (index == 0) {
				modelToUse = startModel_;
			} else if (index == stageCount_ - 1) {
				modelToUse = endModel_;
			} else {
				modelToUse = midModel_;
			}

			stage->Initialize(modelToUse);
			stage->SetPosition(index * stageWidth);
			activeStages_[index] = std::move(stage);
		}
	}
}


void StageManager::Draw(Camera& camera) {
	for (auto& pair : activeStages_) {
		pair.second->Draw(camera);
	}
}
