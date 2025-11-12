#pragma once
#include "Collider.h"

/// <summary>
/// 当たり判定を管理するための関数群をまとめた名前空間
/// </summary>
namespace Collision {

/// <summary>
/// AABB同士の当たり判定
/// </summary>
/// <param name="a">当たり判定1</param>
/// <param name="b">当たり判定2</param>
/// <returns>当たっていればtrue</returns>
inline bool AABB(const HitBox& a, const HitBox& b) {
	if (!a.active || !b.active)
		return false;

	return (fabs(a.pos.x - b.pos.x) < (a.size.x + b.size.x) * 0.5f && fabs(a.pos.y - b.pos.y) < (a.size.y + b.size.y) * 0.5f && fabs(a.pos.z - b.pos.z) < (a.size.z + b.size.z) * 0.5f);
}

} // namespace Collision
