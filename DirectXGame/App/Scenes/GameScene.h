#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine/Camera/CameraController.h"
#include "../Objects/StageManager.h"
#include "../Objects/Player.h"

class Fade;
class GameScene {
public:
	enum class Phase {
		kFadeIn,
		kReady,
		kFight,
		kPlay,
		kFadeOut
	};

public:
	GameScene();

	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() { return isFinished_; }

	void ChangePhase();

	void FightAnimation();

private:
	KamataEngine::DirectXCommon* dxCommon = nullptr;
	KamataEngine::Input* input = nullptr;
	KamataEngine::Audio* audio = nullptr;

	KamataEngine::Camera camera_;
	WorldTransformEx worldTransform_;

	KamataEngine::Model* modelLoad_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* backTextSprite_ = nullptr;
	KamataEngine::Sprite* readyTextSprite_ = nullptr;
	KamataEngine::Sprite* fightTextSprite_ = nullptr;

	StageManager* stage_ = nullptr;
	Player* player_ = nullptr;

	CameraController* cameraController_ = nullptr;

	Fade* fade_ = nullptr;
	float fadeTime_ = 1.0f;

	float startTime_ = 3.0f;
	float deltaTime_ = 1.0f / 60.0f;

	bool isFinished_ = false;

	Phase phase_ = Phase::kFadeIn;

	// ---ファイトテキストアニメーション用---
	float blinkTime_ = 0.0f;
	float fightTextAnimeTimer_ = 0.0f;
	float animeDuration_ = 0.1f; // アニメ時間（秒） — 好みで調整
	float startScale_ = 1.6f;    // 初期スケール（大きめ）
	float waitTimer_ = 0.0f;     // 最初に待つ時間
	float waitDuration_ = 0.5f;  // 1秒待つ
	std::chrono::high_resolution_clock::time_point prevTime_;

	bool fightTextVisible_ = false;
	bool fightTextAnimeFinished_ = false;
	float fadeInTimer_ = 0.0f;
	//bool BGMStarted_ = false;

	KamataEngine::Vector2 fightTextPos_ = {640.0f, 300.0f};
	KamataEngine::Vector2 fightTextSize_ = {400, 150};
	// ---------------------------------
};
