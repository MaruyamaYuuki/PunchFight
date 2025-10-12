#pragma once

#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

class Player {
public:

	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw(KamataEngine::Camera& camera);

	void Move();

private:
	KamataEngine::Input* input_ = nullptr;

	WorldTransformEx worldTransform_;

	KamataEngine::Model* model_ = nullptr;

    bool isStepping_ = false;
	KamataEngine::Vector3 stepDirection_ = {0, 0, 0};
	int stepTimer_ = 0;
	int stepCooldown_ = 30; // 例：1秒クールタイム（60FPS想定）
	int stepFrame_ = 0;
	float stepPower_ = 2.5f; // 通常移動の3倍速
};
