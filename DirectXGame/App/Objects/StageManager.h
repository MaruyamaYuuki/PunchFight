#pragma once
#include "KamataEngine.h"
#include "Stage.h"

class StageManager {
public:
	StageManager() = default;

	~StageManager() = default;

	void Initialize(int stageNumber, int repeatCount);

	void Update();

	void Draw(KamataEngine::Camera& camera);

private:
	std::vector<std::unique_ptr<Stage>> stages_;
	float stageWidth = 20.0f;
};
