#pragma once
#include "Collider.h"

namespace Collision {

inline bool AABB(const HitBox& a, const HitBox& b) {
	if (!a.active || !b.active)
		return false;

	return (fabs(a.pos.x - b.pos.x) < (a.size.x + b.size.x) * 0.5f && fabs(a.pos.y - b.pos.y) < (a.size.y + b.size.y) * 0.5f && fabs(a.pos.z - b.pos.z) < (a.size.z + b.size.z) * 0.5f);
}

} // namespace Collision
