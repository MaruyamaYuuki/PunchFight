#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"

/// <summary>
/// タイトルシーン
/// </summary>
class Fade;
class GameConfigManager;
class TitleScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

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
	/// ゲーム開始時のタイトルロゴのアニメーション処理
	/// </summary>
	void TitleAnimation();

	/// <summary>
	/// タイトルロゴ点滅処理
	/// </summary>
	void SpriteFlashUpdate();

	/// <summary>
	/// タイトルシーンの終了判定
	/// </summary>
	/// <returns>終了していれば true、進行中であれば false を返す。</returns>
	bool IsFinished() { return isFinished_; }

private:
	XINPUT_STATE state_, preState_;

	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	GameConfigManager* cfg_ = nullptr;

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
	float animeDuration_; // アニメ時間（秒） — 好みで調整
	float startScale_;   // 初期スケール（大きめ）
	float waitTimer_ = 0.0f;    // 最初に待つ時間
	float waitDuration_; // 1秒待つ
	float animeEndSize_;
	std::chrono::high_resolution_clock::time_point prevTime_;

	bool titleVisible_ = false;
	bool titleAnimeFinished_ = false;
	float fadeInTimer_ = 0.0f;
	bool titleBGMStarted_ = false;
	float bgmVolume_ ;
	float seVolume_;

	KamataEngine::Vector2 titlePos_;
	KamataEngine::Vector2 titleSize_ ;

	// ---背景スクロール用---
	float bgScrollSpeed_;
	float bgPosX_[2] = {{0.0f}, {1367.0f}};
	float bgResetPosX_;

	// ---シーン遷移用---
	// タイトル点滅制御用
	bool titleBlinking_ = false;
	bool titleBlinkFinished_ = false;
	float blinkTimer_ = 0.0f;
	int32_t blinkCount_ = 0;
	int maxBlinkCount_ ;
	// フェード用
	Fade* fade_ = nullptr;
	float fadeTime_;
	bool isFinished_ = false;



	uint32_t hitSEDataHandle_ = 0;
	uint32_t hitSEVoiceHandle_ = 0;
	uint32_t doubleHitSEDataHandle_ = 0;
	uint32_t doubleHitSEVoiceHandle_ = 0;
	uint32_t titleBGMDataHandle_ = 0;
	uint32_t titleBGMVoiceHandle_ = 0;

	bool isAButtonPressed_ = false;


};
