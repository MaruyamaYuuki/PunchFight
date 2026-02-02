#pragma once
#include "KamataEngine.h"
#include <memory>
#include <json.hpp>
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine/Camera/CameraController.h"
#include "../../Engine/Rendering/UI.h"
#include "../Objects/StageManager.h"
#include "../Objects/Player.h"
#include "../Objects/Enemy/EnemyManager.h"

/// <summary>
/// ゲームシーン
/// </summary>
namespace MyEngine {
class Fade;
class GameConfigManager;
}

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

	bool IsBackToTitle() const { return backToTitle_; }

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
	/// ポーズ開始
	/// </summary>
	void EnterPause();

	/// <summary>
	/// ポーズ終了
	/// </summary>
	void ExitPause();

	/// <summary>
	/// ポーズ画面管理
	/// </summary>
	void UpdatePauseInput();

	/// <summary>
	/// ポーズメニュー処理
	/// </summary>
	void UpdatePauseMenuInput();

	/// <summary>
	/// タイトルへ戻る確認処理
	/// </summary>
	void UpdateBackTitleCheckInput();

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

	/// <summary>
	/// プレイヤーの攻撃→敵の判定
	/// </summary>
	/// <param name="attackHitBox攻撃ヒットボックスparam>
	/// <param name="hitList">ヒットリスト</param>
	/// <param name="attackPower">攻撃力</param>
	/// <param name="attackDir">向いている方向</param>
	void CheckPlayerAttackToEnemies(const HitBox& attackHitBox, std::vector<EnemyBase*>& hitList, int attackPower, float attackDir);

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	MyEngine::GameConfigManager* cfg_ = nullptr;

	KamataEngine::Camera camera_;
	MyEngine::WorldTransformEx worldTransform_;

	std::unique_ptr<KamataEngine::Model> modelPlayer_;
	std::unique_ptr<KamataEngine::Model> modelSPAttack_;
	std::unique_ptr<KamataEngine::Model> modelBoxFrame_;

	uint32_t textureHandle_ = 0;

	std::unique_ptr<KamataEngine::Sprite> backTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> readyTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> fightTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> gameOverTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> blackSprite_;
	std::unique_ptr<KamataEngine::Sprite> resetTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> guideTexture_;

	uint32_t startGongSEDataHandle_ = 0;
	uint32_t bgmDataHandle_ = 0;

	uint32_t startGongSEVoiceHandle_ = 0;
	uint32_t bgmVoiceHandle_ = 0;

	std::unique_ptr<StageManager> stage_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<MyEngine::CameraController> cameraController_;
	std::unique_ptr<MyEngine::Fade> fade_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<MyEngine::UI> ui_;

	std::vector<float> moveLimit_;
	std::vector<float> scrollArea_;
	float cameraLimitZMin_;
	float cameraLimitZMax_;

	uint32_t stageNumber_;
	uint32_t stageRepeatCount_;

	float fadeTime_;

	float kInitialStartTime_;
	float startTime_;
	float deltaTime_ = 1.0f / 60.0f;

	bool isFinished_ = false;

	Phase phase_ = Phase::kFadeIn;

	// ---ファイトテキストアニメーション用---
	float blinkTime_ = 0.0f;
	float fightTextAnimeTimer_ = 0.0f;
	float animeDuration_; // アニメ時間（秒） — 好みで調整
	float startScale_;    // 初期スケール（大きめ）
	float waitTimer_ = 0.0f;     // 最初に待つ時間
	float waitDuration_;  // 0.5秒待つ
	std::chrono::high_resolution_clock::time_point prevTime_;

	bool fightTextVisible_ = false;
	bool fightTextAnimeFinished_ = false;
	float fadeInTimer_ = 0.0f;

	KamataEngine::Vector2 fightTextPos_;
	KamataEngine::Vector2 fightTextSize_;

	// --- ゲームオーバー関連 ---
	float alphaCounter_  = 0.0f;
	float alphaDuration_;
	float gameOverFallTimer_ = 0.0f;    // テキスト落下の経過時間
	float gameOverFallDuration_; // 落下にかける時間（秒）
	bool isGameOverFallFinished_ = false; // 落下完了フラグ

	std::vector<bool> areaClearedFlag_;

	std::vector<EnemyBase*> hitEnemiesThisAttack_;
	std::vector<EnemyBase*> hitEnemiesThisSPAttack_;

	/// --- エリア解放時のガイド関連 ---
	float guideTimer_ = 0.0f;
	float guideDuration_;
	bool guideOn_ = false;
	float blinkInterval_; // 1回の ON/OFF の長さ
	int32_t blinkCount_ = 0;         // 何回点滅したか
	int32_t maxBlinkCount_;      // 合計何回点滅させるか

	/// --- ポーズ処理　---
	bool isPaused_ = false;
	bool isBackTitleChecked_ = false;
	int32_t pauseSelectIndex_ = 0;
	int32_t checkBackTitleIndex_ = 1;
	bool backToTitle_ = false;

	nlohmann::json enemySpawnData_;
};