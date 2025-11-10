#pragma once
#include "KamataEngine.h"

/// <summary>
/// カメラ制御
/// </summary>
class Player;
class CameraController {
public:
	/// <summary>
	/// カメラの移動範囲
	/// </summary>
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

public:
	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	const KamataEngine::Camera& GetCamera() const { return camera_; }

	void Reset();

	void SetMovableArea(Rect area) { movableArea_ = area; }

	KamataEngine::Vector3 LerpVector3(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t);

	KamataEngine::Vector3 GetPosition() const { return camera_.translation_; }

private:
	KamataEngine::Camera camera_;
	Player* target_ = nullptr;
	// 追従対象のカメラ座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 1, -8.0f};
	// カメラの移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
	// カメラの目標座標
	KamataEngine::Vector3 goalPos;
	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;
	// 速度掛け率
	static inline const float kVelocityBias = 2.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect kMargin = {0.0f, 5.0f, -8.0f, 4.0f};
};
