#pragma once
#include "KamataEngine.h"

/// <summary>
/// フェードイン・フェードアウト管理
/// </summary>
class Fade {
public:
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

public:
	~Fade();

	void Initialize();

	void Update();

	void Draw();

	void Start(Status status, float duration);

	void Stop();

	bool IsFinished() const;

private:
	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* sprite_ = nullptr;

	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
	float scale_ = 0.0f;
};
