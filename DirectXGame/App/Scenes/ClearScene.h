#pragma once
#include "KamataEngine.h"
#include "../Objects/Player.h"

/// <summary>
/// クリアシーン
/// </summary>
class Fade;
class ClearScene {
	/// <summary>
	/// クリアシーンの進行状態
	/// </summary>
	enum class Phase { 
		kWaite, 
		kPlay,
		kFadeOut 
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ClearScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ClearScene();

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
	/// ゲームシーンの終了判定
	/// </summary>
	/// <returns>終了していれば true、進行中であれば false を返す。</returns>
	bool IsFinished() const { return isFinished_; }

private:
	KamataEngine::DirectXCommon* dxCommon = nullptr;
	KamataEngine::Input* input = nullptr;
	KamataEngine::Audio* audio = nullptr;

	KamataEngine::Camera camera_;

	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelBoxFrame_ = nullptr;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* backTexture_ = 0;
	KamataEngine::Sprite* spotlightTexture_ = 0;
	KamataEngine::Sprite* frontShadowTexture_ = 0;
	KamataEngine::Sprite* clearTextTexture_ = 0;
	KamataEngine::Sprite* pushSpaceTexture_ = 0;

	Player* player_ = nullptr;

	Fade* fade_ = nullptr;
	float fadeTime_ = 1.0f;

	bool isFinished_ = false;

	float waitTimer_ = 2.0f;
	float deltaTime_ = 1.0f / 60.0f;

	bool isSpot_ = false;

	Phase phase_ = Phase::kWaite;

    float clearScale_ = 0.0f;      // 0 → 1 に拡大させる
	float clearScaleSpeed_ = 2.0f; // 拡大スピード
	bool clearStart_ = false;
	float clearWaitTimer_ = 0.0f; // 拡大後の待機タイマー
	bool pushSpaceShown_ = false; // 一度だけ表示するフラグ
};
