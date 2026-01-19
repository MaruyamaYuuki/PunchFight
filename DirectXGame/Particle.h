#pragma once

#include "KamataEngine.h"
#include "../Math/WorldTransformEx.h"

class Particle {
public:
	virtual ~Particle() = default;

	bool active = false;
	float lifetime = 0.0f;
	float age = 0.0f;
	float alpha = 0.0f;

	WorldTransformEx transform;

	virtual void Update(float deltaTime) = 0;

	virtual void Draw(KamataEngine::Camera& camera) = 0;
};
