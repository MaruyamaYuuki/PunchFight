#include "StageManager.h"
#include <cmath>
#include "../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;

void StageManager::Initialize(int32_t stageNumber, int32_t repeatCount) {
	cfg_ = GameConfigManager::GetInstance();

	stageCount_ = repeatCount;
	currentIndex_ = INT32_MIN;

	// モデルパス設定
	switch (stageNumber) {
	case 1:
		startModel_ = Model::CreateFromOBJ(cfg_->getString("Stage.Models.Stage1.startModelPath"), true);
		midModel_ = Model::CreateFromOBJ(cfg_->getString("Stage.Models.Stage1.midModelPath"), true);
		endModel_ = Model::CreateFromOBJ(cfg_->getString("Stage.Models.Stage1.endModelPath"), true);
		break;

	case 2:
		startModel_ = Model::CreateFromOBJ(cfg_->getString("Stage.Models.Stage2.startModelPath"), true);
		midModel_ = Model::CreateFromOBJ(cfg_->getString("Stage.Models.Stage2.midModelPath"), true);
		endModel_ = Model::CreateFromOBJ(cfg_->getString("Stage.Models.Stage2.endModelPath"), true);
		break;
	}

	stageWidth_ = cfg_->getFloat("Stage.kStageWidth");
}


void StageManager::Update(float cameraX) {
	// スクロールに対して余裕を持たせる
	float preloadOffset = stageWidth_ * 0.3f; // 0.3倍の余裕を持って先読み

	int32_t newIndex = static_cast<int32_t>(std::floor((cameraX + preloadOffset) / stageWidth_));

	if (newIndex != currentIndex_) {
		currentIndex_ = newIndex;
		UpdateLoadedStages();
	}

	for (auto& pair : activeStages_) {
		pair.second->Update();
	}
}

void StageManager::UpdateLoadedStages() {
	std::vector<int32_t> requiredIndices;

	for (int32_t offset = -1; offset <= 1; ++offset) {
		int32_t index = currentIndex_ + offset;
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
	for (int32_t index : requiredIndices) {
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
			stage->SetPosition(index * stageWidth_);
			activeStages_[index] = std::move(stage);
		}
	}
}


void StageManager::Draw(Camera& camera) {
	for (auto& pair : activeStages_) {
		pair.second->Draw(camera);
	}
}
