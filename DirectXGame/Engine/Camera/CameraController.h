#pragma once
#include "KamataEngine.h"

class Player;
namespace MyEngine {
/// <summary>
/// プレイヤーの動きに合わせた視点制御。
/// プレイヤー座標に基づいたスムーズな追従計算（線形補間等）。
/// EnemyManager からの通知に基づいた、進行制限（スクロールロック）の適用。
/// ゲーム進行（エリアクリア）に応じた可動範囲の動的更新。
/// </summary>
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
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラのターゲットの設定
	/// </summary>
	/// <param name="target">ターゲット</param>
	void SetTarget(std::weak_ptr<::Player> target) { target_ = target; }

	/// <summary>
	/// カメラオブジェクトを取得する
	/// </summary>
	/// <returns>エンジンのカメラクラスへの参照</returns>
	const KamataEngine::Camera& GetCamera() const { return camera_; }

	/// <summary>
	/// カメラの状態（座標、補間フラグ等）を初期状態にリセットする
	/// </summary>
	void Reset();

	/// <summary>
	/// カメラの移動範囲を取得する
	/// </summary>
	/// <param name="area">描画するエリア</param>
	void SetMovableArea(Rect area) { movableArea_ = area; }

    /// <summary>
	/// 2つのベクトル間を線形補間（Lerp）する
	/// </summary>
	/// <param name="start">補間の開始座標</param>
	/// <param name="end">補間の終了座標</param>
	/// <param name="t">補間係数（0.0f ～ 1.0f）</param>
	/// <returns>補間後のベクトル</returns>
	KamataEngine::Vector3 LerpVector3(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t);

	/// <summary>
	/// カメラの現在位置を取得する
	/// </summary>
	/// <returns>カメラの現在位置</returns>
	KamataEngine::Vector3 GetPosition() const { return camera_.translation_; }

	/// <summary>
	/// 特定の注視対象からプレイヤーへのカメラ復帰アニメーションを開始する
	/// </summary>
	void StartReturnToPlayer();

private:
	KamataEngine::Camera camera_;
	std::weak_ptr<::Player> target_;
	// 追従対象のカメラ座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 1, -8.0f};
	// カメラの移動範囲
	Rect movableArea_ = {0, 100, 0, 100};
	// 座標補間割合
	static inline const float kInterpolationRate = 0.08f;
	// 速度掛け率
	static inline const float kVelocityBias = 2.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect kMargin = {0.0f, 5.0f, -8.0f, 4.0f};

    bool isReturnInterpolating_ = false;
	KamataEngine::Vector3 returnStartPos_;
	KamataEngine::Vector3 returnTargetPos_;
	float returnT_ = 0.0f;
	KamataEngine::Vector3 velocityBias_{0, 0, 0};
};
}


