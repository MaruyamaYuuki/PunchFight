#pragma once

/// <summary>
/// 全てのシーンの抽象基底クラス
/// </summary>
class BaseScene {
public:
	virtual ~BaseScene() = default;

	// 純粋仮想関数：派生クラスで必ず実装する
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// シーン終了フラグの取得
	virtual bool IsFinished() const = 0;

	// 次にどのシーンへ行くべきかを取得する（追加）
	virtual int GetNextScene() const = 0;
};