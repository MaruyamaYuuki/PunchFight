#include "StageManager.h"

using namespace KamataEngine;

void StageManager::Initialize(int stageNumber, int repeatCount) { 
	std::string modelPath;
	switch (stageNumber) {
	case 1:
		modelPath = "load";
	default:
		break;
	}

	for (int i = 0; i < repeatCount; i++) {
		auto stage = std::make_unique<Stage>();
		stage->Initialize(KamataEngine::Model::CreateFromOBJ(modelPath, true));

		// 横並びに配置
		stage->SetPosition(i * stageWidth);
		stages_.push_back(std::move(stage));
	}
}

void StageManager::Update() {
	for (auto& stage : stages_) {
		stage->Update();
	}
}

void StageManager::Draw(Camera& camera) {
	for (auto& stage : stages_) {
		stage->Draw(camera);
	}
}
