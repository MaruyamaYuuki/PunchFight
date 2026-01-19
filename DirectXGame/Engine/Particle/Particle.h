#pragma once
#include "../Math/WorldTransformEx.h"
#include "KamataEngine.h"

class Particle {
public:
	virtual ~Particle() = default;

	bool active = false;
	float lifetime = 0.0f;
	float age = 0.0f;
	float alpha = 1.0f;

	WorldTransformEx transform;

	virtual void Update(float deltaTime) = 0;
};
