#pragma once
#include "../Math/WorldTransformEx.h"

/// <summary>
/// 煙のパーティクル
/// </summary>
struct SmokeParticle {
	WorldTransformEx transform;
	float lifetime = 0.5f;
	float age = 0.0f;
	float scale = 0.5f;
	float alpha = 1.0f;
	bool active = false;
};
