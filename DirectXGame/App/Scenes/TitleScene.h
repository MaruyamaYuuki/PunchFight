#pragma once
#include "KamataEngine.h"
#include "BaseScene.h"
#include "../../Engine/Math/WorldTransformEx.h"

/// <summary>
/// ゲーム起動時のエントリーポイントおよび待機画面の制御。
/// タイトルロゴ、背景、および操作ガイド（ "Start"など）の描画管理。
/// ロゴの拡大アニメーションや、ボタン入力待ちの点滅演出（SpriteFlashUpdate）の制御。
/// 背景のループスクロール処理による動的な視覚演出の実行。
/// ユーザーの開始入力を検知し、フェードアウト演出を経てメインゲームへの遷移フラグ（isFinished_）を管理する。
/// </summary>

namespace MyEngine {
    class Fade;
    class GameConfigManager;
}
class TitleScene : public BaseScene {
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
	/// シーンの初期化。各種スプライト、カメラ、演出用タイマーの設定を行う。
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// ゲーム開始時のタイトルロゴのアニメーション処理
	/// </summary>
	void TitleAnimation();

	/// <summary>
	/// 「PRESS START」等のガイド表示の点滅制御。
	/// </summary>
	void SpriteFlashUpdate();

	/// <summary>
	/// タイトルシーンの終了判定
	/// </summary>
	/// <returns>終了していれば true、進行中であれば false を返す。</returns>
	bool IsFinished() const override { return isFinished_; }

    /// <summary>
	/// 次の遷移先シーンを取得する
	/// </summary>
	/// <returns>ゲームへ行くフラグが立っていれば kGame</returns>
	int GetNextScene() const override;

private:
	XINPUT_STATE state_, preState_;

	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	KamataEngine::Camera camera_;
	MyEngine::WorldTransformEx worldTransform_;

	uint32_t textureHandle_ = 0;
	uint32_t keyTexture_ = 0;
	uint32_t padTexture_ = 0;

    std::array<std::unique_ptr<KamataEngine::Sprite>, 2> titleBackSprite_;
	std::unique_ptr<KamataEngine::Sprite> titleSprite_;
	std::unique_ptr<KamataEngine::Sprite> startSprite_;
	std::unique_ptr<KamataEngine::Sprite> fadeSprite_;
	std::unique_ptr<KamataEngine::Sprite> loadingTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> loadingBarSprite_;

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
	float floatingTimer_ = 0.0f;

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
	KamataEngine::Vector2 baseStartSize_;
	// フェード用
	std::unique_ptr<MyEngine::Fade> fade_;
	float fadeTime_;
	bool isFinished_ = false;

	uint32_t hitSEDataHandle_ = 0;
	uint32_t hitSEVoiceHandle_ = 0;
	uint32_t doubleHitSEDataHandle_ = 0;
	uint32_t doubleHitSEVoiceHandle_ = 0;
	uint32_t titleBGMDataHandle_ = 0;
	uint32_t titleBGMVoiceHandle_ = 0;

	bool isBButtonPressed_ = false;

	bool isPadConnected_ = false;
};