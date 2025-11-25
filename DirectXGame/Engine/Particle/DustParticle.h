#pragma once
#include "../Math/WorldTransformEx.h"

/// <summary>
/// 粉塵パーティクル
/// </summary>
struct DustParticle {
	WorldTransformEx transform;
	float lifetime = 0.4f; // パーティクル生存時間
	float age = 0.0f;      // 経過時間
	float scale = 0.3f;    // 初期スケール
	float alpha = 1.0f;    // 透明度
	bool active = false;   // 使用中かどうか

	// 追加で風っぽい揺れを付けたい時用
	float velocityX = 0.0f;
	float velocityY = 0.02f;
};
