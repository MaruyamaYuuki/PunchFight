#define NOMINMAX
#include "CameraController.h"
#include "../../App/Objects/Player/Player.h"
#include "../Math/WorldTransformEx.h"
#include <algorithm>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

namespace MyEngine {
void CameraController::Initialize() { 
	camera_.Initialize(); 
	camera_.rotation_.x += 0.4f;
}

void CameraController::Update() {
	if (!target_) {
		return;
	}

	const WorldTransform& targetWT = target_->GetWorldTransform();
	//const Vector3& targetVelocity = target_->GetVelocity();

    // ===== 基本追従位置 =====
	Vector3 followPos;

	// X：常にプレイヤー中心
	followPos.x = targetWT.translation_.x + targetOffset_.x;

	// Y：固定
	followPos.y = 4.0f;

	// Z：完全固定（速度もプレイヤー位置も見ない）
	followPos.z = targetOffset_.z;

	// ===== 中央復帰の補間 =====
	if (isReturnInterpolating_) {
		returnT_ += kInterpolationRate;
		if (returnT_ > 1.0f) {
			returnT_ = 1.0f;
		}

		camera_.translation_ = LerpVector3(returnStartPos_, followPos, returnT_);

		if (returnT_ >= 1.0f) {
			isReturnInterpolating_ = false;
		}
	}
	// ===== 通常時（即追従） =====
	else {
		camera_.translation_ = followPos;
	}

	// ===== 移動制限 =====
	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);

	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}

KamataEngine::Vector3 CameraController::LerpVector3(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t) { return start + t * (end - start); }

void CameraController::StartReturnToPlayer() {
	returnStartPos_ = camera_.translation_;
	returnT_ = 0.0f;
	isReturnInterpolating_ = true;
}
}

