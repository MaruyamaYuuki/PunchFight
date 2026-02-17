#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

/// <summary>
/// ステージを構成する最小単位（1セクション）の保持と表示。
/// 単一のステージモデルと、その配置座標（WorldTransformEx）の管理。
/// モデルの描画、および個別のトランスフォーム更新。
/// マネージャーから指定された座標への自己配置。
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
	/// ステージセクションの初期化
	/// </summary>
	/// <param name="model">このセクションで使用する3Dモデルのポインタ</param>
	void Initialize(KamataEngine::Model* model);

	/// <summary>
	/// ワールド行列の更新
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
	/// <param name="x">配置するX座標（進行方向）</param>
	/// <param name="y">配置するY座標（高さ・省略時は0.0f）</param>
	/// <param name="z">配置するZ座標（奥行・省略時は0.0f）</param>
	void SetPosition(float x, float y = 0.0f, float z = 0.0f) { worldTransform_.translation_ = {x, y, z}; }

	/// <summary>
	///ステージのX座標を取得する
	/// </summary>
	/// <returns>ステージの現在のX座標</returns>
	float GetPositionX() const { return worldTransform_.translation_.x; }

private:
	MyEngine::WorldTransformEx worldTransform_;

	KamataEngine::Model* model_ = nullptr;
};
