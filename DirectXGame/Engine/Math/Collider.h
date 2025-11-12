#pragma once
#include "KamataEngine.h"

/// <summary>
/// 当たり判定用
/// </summary>
struct HitBox {
	bool active = false;
	KamataEngine::Vector3 pos{};
	KamataEngine::Vector3 size{};
};
