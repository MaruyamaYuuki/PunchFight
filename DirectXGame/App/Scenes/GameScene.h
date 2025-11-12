#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine/Camera/CameraController.h"
#include "../Objects/StageManager.h"
#include "../Objects/Player.h"
#include "../Objects/Enemy/EnemyManager.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Fade;
class GameScene {
public:
	/// <summary>
	/// ゲーム全体の進行状態
	/// </summary>
	enum class Phase {
		kFadeIn,
		kReady,
		kFight,
		kPlay,
		kFadeOut
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

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
	/// <summary>
	/// シーン切り替え
	/// </summary>
	void ChangePhase();

	/// <summary>
	/// 開始時のアニメーション
	/// </summary>
	void FightAnimation();

	/// <summary>
	/// ゲームオーバー時の演出
	/// </summary>
	void GameOver();

	/// <summary>
	/// ゲームの状態をリセット
	/// </summary>
	void ResetGame();

	/// <summary>
	/// 敵の生成
	/// </summary>
	void EnemyGenerate();

	/// <summary>
	/// 敵の更新
	/// </summary>
	void EnemyUpdate();

	/// <summary>
	/// 当たり判定
	/// </summary>
	void AllCollision();

private:
	KamataEngine::DirectXCommon* dxCommon = nullptr;
	KamataEngine::Input* input = nullptr;
	KamataEngine::Audio* audio = nullptr;

	KamataEngine::Camera camera_;
	WorldTransformEx worldTransform_;

	KamataEngine::Model* modelLoad_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelBoxFrame_ = nullptr;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* backTextSprite_ = nullptr;
	KamataEngine::Sprite* readyTextSprite_ = nullptr;
	KamataEngine::Sprite* fightTextSprite_ = nullptr;
	KamataEngine::Sprite* gameOverTextSprite_ = nullptr;
	KamataEngine::Sprite* blackSprite_ = nullptr;
	KamataEngine::Sprite* resetTextSprite_ = nullptr;
	KamataEngine::Sprite* guideTexture_ = nullptr;

	uint32_t startGongSEDataHandle_ = 0;
	uint32_t startGongSEVoiceHandle_ = 0;

	StageManager* stage_ = nullptr;
	Player* player_ = nullptr;
	float moveLimit[3] = {20.0f, 35.0f, 50.0f};

	CameraController* cameraController_ = nullptr;

	Fade* fade_ = nullptr;
	float fadeTime_ = 1.0f;

	float startTime_ = 3.0f;
	float deltaTime_ = 1.0f / 60.0f;

	bool isFinished_ = false;

	Phase phase_ = Phase::kFadeIn;

	EnemyManager* enemyManager_ = nullptr;

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
	float alphaCounter_  = 0.0f;
	float duration_ = 1.0f;
	float gameOverFallTimer_ = 0.0f;    // テキスト落下の経過時間
	float gameOverFallDuration_ = 1.0f; // 落下にかける時間（秒）
	bool isGameOverFallFinished_ = false; // 落下完了フラグ

	bool areaClearedFlag_[3] = {false, false, false};

	float scrollArea[3] = {15.0f, 30.0f, 45.0f};

	std::vector<EnemyBase*> hitEnemiesThisAttack_;

	float guideTimer_ = 0.0f;
	float guideDuration_ = 3.0f;
	bool guideOn_ = false;
	float blinkInterval_ = 1.0f; // 1回の ON/OFF の長さ
	int blinkCount_ = 0;         // 何回点滅したか
	int maxBlinkCount_ = 3;      // 合計何回点滅させるか
};