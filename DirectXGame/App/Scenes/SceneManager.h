#pragma once
#include "BaseScene.h"
#include <memory>

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneManager {
public:
	// シーンの種類を定義
	enum class SceneType { kTitle, kGame, kClear };

	/// <summary>
	/// シーンの更新。終了判定をチェックし、必要なら切り替えを行う。
	/// </summary>
	void Update();

	/// <summary>
	/// 現在のシーンの描画。
	/// </summary>
	void Draw();

	/// <summary>
	/// 指定したシーンへ切り替える。
	/// </summary>
	void ChangeScene(SceneType nextScene);

private:
	std::unique_ptr<BaseScene> currentScene_; // 現在のシーン
};