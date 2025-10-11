#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

class Stage {
public:
	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw(KamataEngine::Camera& camera);

private:
	WorldTransformEx worldTransform_;

	KamataEngine::Model* model_ = nullptr;
};
