#pragma once
#define NOMINMAX
#include "Collider.h"
#include <algorithm>

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

/// <summary>
/// 円形とAABBの当たり判定
/// </summary>
/// <param name="center">円の中心座標</param>
/// <param name="radius">円の半径</param>
/// <param name="box">当たり判定のAABB</param>
/// <returns>当たっていればtrue</returns>
inline bool CircleVsAABB(const KamataEngine::Vector3& center, float radius, const HitBox& box) {
	if (!box.active)
		return false;

	float minX = box.pos.x - box.size.x * 0.5f;
	float maxX = box.pos.x + box.size.x * 0.5f;

	float minZ = box.pos.z - box.size.z * 0.5f;
	float maxZ = box.pos.z + box.size.z * 0.5f;

	float closestX = std::clamp(center.x, minX, maxX);
	float closestZ = std::clamp(center.z, minZ, maxZ);

	float dx = center.x - closestX;
	float dz = center.z - closestZ;

	return dx * dx + dz * dz <= radius * radius;
}

/// <summary>
/// リング状（ドーナツ型）とAABBの当たり判定
/// </summary>
/// <param name="center">円の中心座標</param>
/// <param name="radius">外円の半径（現在の衝撃波の広がり）</param>
/// <param name="thickness">衝撃波の当たり判定の厚み（すり抜け防止）</param>
/// <param name="box">当たり判定のAABB</param>
/// <returns>当たっていればtrue</returns>
inline bool RingVsAABB(const KamataEngine::Vector3& center, float radius, float thickness, const HitBox& box) {
	if (!box.active)
		return false;

	float minX = box.pos.x - box.size.x * 0.5f;
	float maxX = box.pos.x + box.size.x * 0.5f;

	float minZ = box.pos.z - box.size.z * 0.5f;
	float maxZ = box.pos.z + box.size.z * 0.5f;

	// 1. 円中心からAABBへの「最も近い点」の距離の2乗を計算
	float closestX = std::clamp(center.x, minX, maxX);
	float closestZ = std::clamp(center.z, minZ, maxZ);
	float dxMin = center.x - closestX;
	float dzMin = center.z - closestZ;
	float distSqMin = dxMin * dxMin + dzMin * dzMin;

	// 2. 円中心からAABBへの「最も遠い点」の距離の2乗を計算
	float farX = (std::abs(center.x - minX) > std::abs(center.x - maxX)) ? minX : maxX;
	float farZ = (std::abs(center.z - minZ) > std::abs(center.z - maxZ)) ? minZ : maxZ;
	float dxMax = center.x - farX;
	float dzMax = center.z - farZ;
	float distSqMax = dxMax * dxMax + dzMax * dzMax;

	// 外円と内円の半径の2乗を計算
	float outerRadiusSq = radius * radius;

	// 内円の半径（半径 - 厚み）。0未満にならないようクランプする
	float innerRadius = radius - thickness;
	if (innerRadius < 0.0f) {
		innerRadius = 0.0f;
	}
	float innerRadiusSq = innerRadius * innerRadius;

	// AABBの最も近い点が外円の内側にあり、かつ、最も遠い点が内円の外側にあれば交差している
	return (distSqMin <= outerRadiusSq) && (distSqMax >= innerRadiusSq);
}

} // namespace Collision
