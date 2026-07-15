#pragma once
#include "KamataEngine.h"
#include "BaseScene.h"
#include "../Objects/Player/Player.h"

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
class ClearScene : public BaseScene {
	/// <summary>
	/// クリアシーンの進行状態
	/// </summary>
	enum class Phase { 
		kWait, 
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
	/// シーンの初期化。勝利ポーズ用のプレイヤーモデル生成やクリア演出用タイマーのリセットを行う。
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
	/// <returns>終了していれば true、進行中であれば false を返す。</returns>
	bool IsFinished() const override { return isFinished_; }

    /// <summary>
	/// 次の遷移先シーンを取得する
	/// </summary>
	/// <returns>タイトルへ戻るフラグが立っていれば kTitle</returns>
	int GetNextScene() const override;

private:

	/// <summary>
	/// クリア直後の演出待機フェーズ。一定時間経過後にテキスト演出を開始する。
	/// </summary>
	void UpdateWait();

	/// <summary>
	/// メインの演出フェーズ。クリアテキストの表示やプレイヤーのアニメーションを行う。
	/// </summary>
	void UpdatePlay();

	/// <summary>
	/// 「CLEAR」ロゴの拡大・バウンド等の視覚効果アニメーション。
	/// </summary>
	void UpdateClearText();

	/// <summary>
	/// 演出完了後のユーザー入力を監視。ボタン入力によりフェードアウトへ遷移させる。
	/// </summary>
	void UpdateInput();

	/// <summary>
	/// タイトルシーン等へ戻る際の暗転（フェードアウト）処理。
	/// </summary>
	void UpdateFadeOut();

private:
	XINPUT_STATE state_, preState_;

	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	MyEngine::GameConfigManager* cfg_ = nullptr;

	KamataEngine::Camera camera_;

	std::unique_ptr<KamataEngine::Model> modelPlayer_;
	std::unique_ptr<KamataEngine::Model> modelBoxFrame_;

	uint32_t textureHandle_ = 0;
	uint32_t keyPushTexture_ = 0;
	uint32_t padPushTexture_ = 0;

	std::unique_ptr<KamataEngine::Sprite> backSprite_ = 0;
	std::unique_ptr<KamataEngine::Sprite> clearSprite_ = 0;
	std::unique_ptr<KamataEngine::Sprite> pushGuideSprite_ = 0;

	std::unique_ptr<Player> player_;

	std::unique_ptr<MyEngine::Fade> fade_;
	float fadeTime_ = 1.0f;

	bool isFinished_ = false;

	float waitTimer_ = 2.0f;
	float deltaTime_ = 1.0f / 60.0f;

	Phase phase_ = Phase::kWait;

    float clearScale_ = 0.0f;      // 0 → 1 に拡大させる
	float clearScaleSpeed_ = 2.0f; // 拡大スピード
	bool clearStart_ = false;
	float clearWaitTimer_ = 0.0f; // 拡大後の待機タイマー
	bool pushSpaceShown_ = false; // 一度だけ表示するフラグ

	bool isAButtonPressed_ = false;
	bool isPadConnected_ = false;
};
