#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

/// <summary>
/// タイトルシーン
/// </summary>
class Fade;
class TitleScene {
public:
	TitleScene();

	~TitleScene();

	void Initialize();

	void Update();

	void Draw();

	void TitleAnimation();

	void SpriteFlashUpdate();

	void FadeOutUpdate();
	
	void NowLoading();

	bool IsFinished() { return isFinished_; }

private:
	XINPUT_STATE state, preState;

	KamataEngine::DirectXCommon* dxCommon = nullptr;
	KamataEngine::Input* input = nullptr;
	KamataEngine::Audio* audio = nullptr;

	KamataEngine::Camera camera_;
	WorldTransformEx worldTransform_;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* titleBackSprite[2];
	KamataEngine::Sprite* titleSprite_ = nullptr;
	KamataEngine::Sprite* startSprite_ = nullptr;
	KamataEngine::Sprite* fadeSprite_ = nullptr;
	KamataEngine::Sprite* loadingTextSprite_ = nullptr;
	KamataEngine::Sprite* loadingBarSprite_ = nullptr;

	// ---タイトルロゴアニメーション用---
	float blinkTime_ = 0.0f;
	float titleAnimeTimer_ = 0.0f;
	float animeDuration_ = 0.4f; // アニメ時間（秒） — 好みで調整
	float startScale_ = 1.6f;   // 初期スケール（大きめ）
	float waitTimer_ = 0.0f;    // 最初に待つ時間
	float waitDuration_ = 0.5f; // 1秒待つ
	std::chrono::high_resolution_clock::time_point prevTime_;

	bool titleVisible_ = false;
	bool titleAnimeFinished_ = false;
	float fadeInTimer_ = 0.0f;
	bool titleBGMStarted_ = false;

	KamataEngine::Vector2 titlePos_ = {640.0f, 200.0f};
	KamataEngine::Vector2 titleSize_ = {600, 364};
	// ---------------------------------

	// ---背景スクロール用---
	float bgScrollSpeed_ = 1.0f;
	float bgPosX[2] = {{0.0f}, {1367.0f}};
	// -----------------------

	// ---シーン遷移用---
	// タイトル点滅制御用
	bool titleBlinking_ = false;
	bool titleBlinkFinished_ = false;
	float blinkTimer_ = 0.0f;
	int blinkCount_ = 0;
	const int kMaxBlinkCount_ = 2;
	// フェード用
	Fade* fade_ = nullptr;
	float fadeTime_ = 1.0f;
	bool isFinished_ = false;
	// -----------------

	uint32_t hitSEDataHandle_ = 0;
	uint32_t hitSEVoiceHandle_ = 0;
	uint32_t doubleHitSEDataHandle_ = 0;
	uint32_t doubleHitSEVoiceHandle_ = 0;
	uint32_t titleBGMDataHandle_ = 0;
	uint32_t titleBGMVoiceHandle_ = 0;

	bool isAButtonPressed = false;


};
