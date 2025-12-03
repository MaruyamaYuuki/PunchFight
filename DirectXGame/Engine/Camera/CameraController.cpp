#define NOMINMAX
#include "CameraController.h"
#include "../../App/Objects/Player.h"
#include "../Math/WorldTransformEx.h"
#include <algorithm>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void CameraController::Initialize() { 
	camera_.Initialize(); 
	camera_.rotation_.x += 0.4f;
}

void CameraController::Update() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットからカメラの目標座標を計算
	goalPos_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias_;

	// --- Z軸だけ固定したい場合 ---
	goalPos_.z = targetOffset_.z;

	// 座標補間でスムーズ追従
	camera_.translation_ = LerpVector3(camera_.translation_, goalPos_, kInterpolationRate_);

	// 移動範囲制限
	camera_.translation_.x = std::max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = std::min(camera_.translation_.x, movableArea_.right);

	camera_.translation_.y = 4.0f;

	// 行列更新
	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}

KamataEngine::Vector3 CameraController::LerpVector3(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t) { return start + t * (end - start); }
