#pragma once
#include "KamataEngine.h"

struct HitBox {
	bool active = false;
	KamataEngine::Vector3 pos{};
	KamataEngine::Vector3 size{};
};
