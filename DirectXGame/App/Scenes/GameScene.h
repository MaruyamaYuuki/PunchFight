#pragma once
#include "KamataEngine.h"
#include <memory>
#include <json.hpp>
#include "BaseScene.h"
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine/Camera/CameraController.h"
#include "../../Engine/Rendering/UI.h"
#include "../../Engine/Rendering/UIInputController.h"
#include "../Objects/StageManager.h"
#include "../Objects/Player/Player.h"
#include "../Objects/Enemy/EnemyManager.h"

/// <summary>
/// メインゲームのロジック実行と、複雑なゲームプレイ要素の統合管理。
/// 状態遷移管理 : 導入（Fight演出）、プレイ中、ポーズ、ゲームオーバー等の内部フェーズ（Phase）の制御。
/// オブジェクト連携 : プレイヤー、敵（Manager）、ステージ、カメラ、UIの各インスタンスの生成と、それら相互のデータ受け渡し。
/// 進行制御 : JSONデータ（enemySpawnData_）に基づいた敵の生成トリガー、およびエリアクリアによる進行制限の解除。
/// システム処理 : 全オブジェクト間の当たり判定（AllCollision）の実行と、ポーズメニュー等のシステム入力の監視。
/// </summary>
namespace MyEngine {
class Fade;
class GameConfigManager;
}

class GameScene : public BaseScene{
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
	/// ゲームシーンの初期化。
	/// JSONからの敵出現データの読み込み、プレイヤー・敵・ステージ各マネージャーの生成と初期設定を行う。
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
	/// ゲームシーンの終了判定
	/// </summary>
	/// <returns>シーン遷移が必要な状態（フェードアウト完了後等）であれば true</returns>
	bool IsFinished() const override { return isFinished_; }

	/// <summary>
	/// タイトル画面への戻り判定
	/// </summary>
	/// <returns>ポーズメニュー等で「タイトルへ戻る」が選択された場合 true</returns>
	bool IsBackToTitle() const { return backToTitle_; }

    /// <summary>
	/// 次の遷移先シーンを取得する
	/// </summary>
	/// <returns>タイトルへ戻るフラグが立っていれば kTitle、そうでなければ（クリア等） kClear</returns>
	int GetNextScene() const override;

private:
	/// <summary>
	/// 内部フェーズ（Phase）の遷移管理。
	/// フェード完了時やアニメーション終了時など、条件を満たした際に次のステートへ移行させる。
	/// </summary>
	void ChangePhase();

	/// <summary>
	/// 開幕の「READY...FIGHT!」演出の更新。
	/// テキストの拡大・縮小アニメーションと、終了後のプレイフェーズへの遷移を行う。
	/// </summary>
	void FightAnimation();

	/// <summary>
	/// ポーズ状態への移行。
	/// ゲームタイマーの停止や、ポーズメニューUIの表示フラグを立てる。
	/// </summary>
	void EnterPause();

	/// <summary>
	/// ポーズ状態の解除。ゲームプレイを再開する。
	/// </summary>
	void ExitPause();

	/// <summary>
	/// ボース処理の入力監視
	/// </summary>
	void UpdatePauseInput();

	/// <summary>
	/// ポーズメニュー内の選択操作（上下移動や決定）を監視・実行する。
	/// </summary>
	void UpdatePauseMenuInput();

	/// <summary>
	/// タイトルへ戻る確認処理
	/// </summary>
	void UpdateBackTitleCheckInput();

	/// <summary>
	/// プレイヤー死亡時のスロー演出や、ゲームオーバーテキストの落下アニメーションを制御。
	/// </summary>
	void GameOver();

	/// <summary>
	/// プレイヤーの再生成、敵の全削除、エリアフラグのリセットを行い、ゲームを最初からやり直す。
	/// </summary>
	void ResetGame();

	/// <summary>
	/// 読み込まれた敵出現データ（enemySpawnData_）とカメラ座標を照合し、
	/// 条件を満たしたエリアに敵を動的に生成する。
	/// </summary>
	void EnemyGenerate();

	/// <summary>
	/// 敵の更新
	/// </summary>
	void EnemyUpdate();
	
	/// <summary>
	/// 全オブジェクト間の衝突判定を一括実行。
	/// プレイヤー攻撃vs敵、敵攻撃vsプレイヤー、エリア進入判定などを含む。
	/// </summary>
	void AllCollision();

    /// <summary>
	/// プレイヤーの攻撃ヒット判定とダメージ処理の実行
	/// </summary>
	/// <param name="attackHitBox">プレイヤーの攻撃判定用ボックス</param>
	/// <param name="hitList">既にヒット済みの敵を重複ヒット防止のために格納するリスト</param>
	/// <param name="attackPower">敵に与えるダメージ量</param>
	/// <param name="attackDir">攻撃の向き（1.0f または -1.0f。ノックバック方向に使用）</param>
	void CheckPlayerAttackToEnemies(const HitBox& attackHitBox, std::vector<EnemyBase*>& hitList, int attackPower, float attackDir);

    /// <summary>
	/// 現在のステージ内の全エリア（全エネミーの撃破等）がクリアされたかを判定する。
	/// </summary>
	/// <returns>全ての条件を満たしていれば true、未完了なら false</returns>
	bool IsAllAreaCleared() const;

	/// <summary>
	/// 次のステージへの遷移処理を実行する。
	/// ステージカウントの加算や、ステージ間を繋ぐフェード演出等のトリガーを行う。
	/// </summary>
	void GoToNextStage();

	/// <summary>
	/// 次のステージ開始に向けたリセット処理。
	/// 旧ステージの敵リストの破棄、エリアフラグの初期化、プレイヤーの初期配置設定等を行う。
	/// </summary>
	void ResetForNextStage();

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	MyEngine::GameConfigManager* cfg_ = nullptr;
	std::unique_ptr<MyEngine::UIInputController> uiInput_;
	UICommand cmd;

	KamataEngine::Camera camera_;
	MyEngine::WorldTransformEx worldTransform_;

	std::unique_ptr<KamataEngine::Model> modelPlayer_;
	std::unique_ptr<KamataEngine::Model> modelSPAttack_;
	std::unique_ptr<KamataEngine::Model> modelBoxFrame_;

	uint32_t textureHandle_ = 0;

	std::unique_ptr<KamataEngine::Sprite> readyTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> fightTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> gameOverTextSprite_;
	std::unique_ptr<KamataEngine::Sprite> blackSprite_;
	std::unique_ptr<KamataEngine::Sprite> guideTexture_;
	std::unique_ptr<KamataEngine::Sprite> gameOverTextGuideSprite_;
	std::unique_ptr<KamataEngine::Sprite> gameOverSelectSprite_[2];
	std::unique_ptr<KamataEngine::Sprite> gameOverSelectToRestartSprite_;
	std::unique_ptr<KamataEngine::Sprite> gameOverSelectToTitleSprite_;

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
	uint32_t maxStageNumber_;

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
	bool isGameOverSelect_ = false;
	int32_t gameOverSelectIndex_ = 0;

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