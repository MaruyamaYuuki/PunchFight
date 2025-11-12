#pragma once
#include "KamataEngine.h"

/// <summary>
/// WorldTransformの拡張
/// </summary>
class WorldTransformEx : 
	public KamataEngine::WorldTransform {

public:
	/// <summary>
	/// Affine変換行列の生成と定数バッファへの転送を行う
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// Affine変換行列の生成
	/// </summary>
	KamataEngine::Matrix4x4 MakeAffinMatrix();
};
