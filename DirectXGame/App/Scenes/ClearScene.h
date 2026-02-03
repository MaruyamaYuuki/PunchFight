#pragma once
#include "KamataEngine.h"
#include "../Objects/Player.h"

/// <summary>
/// ゲームクリア時のリザルト表示と、報酬演出の提供。
/// ステージクリアを称えるテキスト（clearTextTexture_）の拡大・演出処理。
/// プレイヤーキャラクターの勝利ポーズや特定の待機アニメーションの表示管理。
/// 演出完了後の入力待ち状態の制御と、タイトルシーンへ戻るためのフェードアウト処理。
/// 一定時間の待機フェーズ（Phase::kWaite）による、クリアの余韻を作るためのシーケンス管理。
/// </summary>

namespace MyEngine {
    class Fade;
    class GameConfigManager;
}
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

	/// <summary>
	/// 待機フェースの更新
	/// </summary>
	void UpdateWait();

	/// <summary>
	/// プレイフェースの更新
	/// </summary>
	void UpdatePlay();

	/// <summary>
	/// クリアテキストの更新
	/// </summary>
	void UpdateClearText();

	/// <summary>
	/// 入力処理の更新
	/// </summary>
	void UpdateInput();

	/// <summary>
	/// フェードアウトフェーズの更新
	/// </summary>
	void UpdateFadeOut();

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	MyEngine::GameConfigManager* cfg_ = nullptr;

	KamataEngine::Camera camera_;

	std::unique_ptr<KamataEngine::Model> modelPlayer_;
	std::unique_ptr<KamataEngine::Model> modelBoxFrame_;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* backTexture_ = 0;
	KamataEngine::Sprite* clearTextTexture_ = 0;
	KamataEngine::Sprite* pushSpaceTexture_ = 0;

	std::unique_ptr<Player> player_;

	std::unique_ptr<MyEngine::Fade> fade_;
	float fadeTime_ = 1.0f;

	bool isFinished_ = false;

	float waitTimer_ = 2.0f;
	float deltaTime_ = 1.0f / 60.0f;

	Phase phase_ = Phase::kWaite;

    float clearScale_ = 0.0f;      // 0 → 1 に拡大させる
	float clearScaleSpeed_ = 2.0f; // 拡大スピード
	bool clearStart_ = false;
	float clearWaitTimer_ = 0.0f; // 拡大後の待機タイマー
	bool pushSpaceShown_ = false; // 一度だけ表示するフラグ
};
