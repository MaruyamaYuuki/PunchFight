#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw(KamataEngine::Camera& camera);

	void SetPosition(float x, float y = 0.0f, float z = 0.0f) { worldTransform_.translation_ = {x, y, z};
	}

private:
	WorldTransformEx worldTransform_;

	KamataEngine::Model* model_ = nullptr;
};
