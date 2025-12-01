#pragma once
#include "../Math/WorldTransformEx.h"
#include "KamataEngine.h"

/// <summary>
/// 煙のパーティクル
/// </summary>
struct SmokeParticle {
	WorldTransformEx transform;
	KamataEngine::Vector3 initialScale;
	float lifetime = 0.5f;
	float age = 0.0f;
	float scale = 0.5f;
	float alpha = 1.0f;
	bool active = false;
};
