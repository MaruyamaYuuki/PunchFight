#pragma once
#include "KamataEngine.h"

class Player;
namespace MyEngine {
/// <summary>
/// ゲームの進行状況やプレイヤーのステータスを視覚的にフィードバックする。
/// 情報の可視化 : プレイヤーの現在HPやスキルのクールタイム（UpdateAbilityCoolTime）を、スプライトのスケールや色の変化として描画する。
///システム状態の同期 : ポーズ画面（isPaused_）やタイトル確認（isBackTitleChecked_）の状態に応じて、メニュー項目やカーソル位置を動的に更新する。
/// デバイス抽象化の補助 : キーボードとゲームパッド（keyCTRLSprite_ /
/// padCTRLSprite_）の操作ガイドを動的に切り替え、ユーザーに適切な操作方法を提示する。
/// GameConfigManagerとの連携 : 外部設定に基づいたUIの配置やパラメータの適用。
/// </summary>
class GameConfigManager;
class UI {
public:

    /// <summary>
	/// UIの初期化
	/// </summary>
	/// <param name="player">HP等のステータスを参照するためのプレイヤーオブジェクト</param>
	void Initialize(::Player* player);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// リセット処理
	/// </summary>
	void Reset();

    /// ポーズ状態の設定
	/// </summary>
	/// <param name="pause">ポーズ中なら true、解除なら false</param>
	void SetPause(bool pause) { isPaused_ = pause; }

	/// <summary>
	/// タイトル確認画面の表示設定
	/// </summary>
	/// <param name="check">確認ダイアログを表示するなら true</param>
	void SetBackTitleCheck(bool check) { isBackTitleChecked_ = check; }

    /// <summary>
	/// ポーズメニューの選択インデックス設定
	/// </summary>
	/// <param name="index">選択中の項目の番号</param>
	void SetPauseSelectIndex(int32_t index) { pauseSelectIndex_ = index; }

	/// <summary>
	/// タイトル確認画面の選択インデックス設定
	/// </summary>
	/// <param name="index">選択中の項目の番号</param>
	void SetCheckBackTitleIndex(int32_t index) { checkBackTitleIndex_ = index; }
private:

	/// <summary>
	/// HPバーの更新
	/// </summary>
	void UpdateHPBar();

	/// <summary>
	/// アビリティアイコンの更新
	/// </summary>
	void UpdateAbilityCoolTime();

private:
	GameConfigManager* cfg_ = nullptr;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* hpBarSprite_ = nullptr;
	KamataEngine::Sprite* hpSprite_ = nullptr;
	KamataEngine::Sprite* hpBackSprite_ = nullptr;
	KamataEngine::Sprite* dashShadowSprite_ = nullptr;
	KamataEngine::Sprite* spShadowSprite_ = nullptr;
	KamataEngine::Sprite* keyCTRLSprite_ = nullptr;
	KamataEngine::Sprite* padCTRLSprite_ = nullptr;
	KamataEngine::Sprite* ctrlSprite_ = nullptr;
	KamataEngine::Sprite* pauseSprite_ = nullptr;
	KamataEngine::Sprite* selectSprite_[2];
	KamataEngine::Sprite* selectCursorSprite_[2];
	KamataEngine::Sprite* checkFrameSprite_ = nullptr;
	KamataEngine::Sprite* checkSelectSprite_[2];
	KamataEngine::Sprite* checkCursorSprite_[2];
	KamataEngine::Sprite* selectKeySprite_[2];

	KamataEngine::Vector2 dashIconSize_ = {64.0f, 64.0f};
	KamataEngine::Vector2 spIconSize = {64.0f, 64.0f};

	bool isPaused_ = false;
	bool isBackTitleChecked_ = false;
	int32_t pauseSelectIndex_ = 0;
	int32_t checkBackTitleIndex_ = 0;

	KamataEngine::Vector2 selectPos_[2] = {
	    {640.0f,  240.0f},
        {640.0f, 360.0f}
    };
	KamataEngine::Vector2 checkPos_ = {640.0f, 330.0f};
	KamataEngine::Vector2 selectKeyPos_ = {115.0f, 683.0f};

	Player* player_ = nullptr;
};
}

