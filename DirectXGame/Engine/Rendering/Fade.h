#pragma once
#include "KamataEngine.h"

/// <summary>
/// フェードイン・フェードアウト管理
/// </summary>
class Fade {
public:
	/// <summary>
	/// フェードの状態
	/// </summary>
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Fade();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フェードの開始処理
	/// </summary>
	/// <param name="status">フェードの種類</param>
	/// <param name="duration">フェードの時間</param>
	void Start(Status status, float duration);

	/// <summary>
	/// フェードの終了処理
	/// </summary>
	void Stop();

	/// <summary>
	/// フェードの終了判定
	/// </summary>
	/// <returns>
	/// <returns>終了していれば true、進行中であれば false を返す。</returns>
	bool IsFinished() const;

private:
	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* sprite_ = nullptr;

	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
	float scale_ = 0.0f;
};
