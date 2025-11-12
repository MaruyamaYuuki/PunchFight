#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

/// <summary>
/// ステージ
/// </summary>
class Stage {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	Stage() = default;

	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~Stage() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(KamataEngine::Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

    /// <summary>
	/// ステージの位置を設定する
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標（省略時は0.0f）</param>
	/// <param name="z">Z座標（省略時は0.0f）</param>
	void SetPosition(float x, float y = 0.0f, float z = 0.0f) { worldTransform_.translation_ = {x, y, z};}

	/// <summary>
	///ステージのX座標を取得する
	/// </summary>
	/// <returns>ステージの現在のX座標</returns>
	float GetPositionX() const { return worldTransform_.translation_.x; }

private:
	WorldTransformEx worldTransform_;

	KamataEngine::Model* model_ = nullptr;
};
