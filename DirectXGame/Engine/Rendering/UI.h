#pragma once
#include "KamataEngine.h"
#include <memory>

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
	void Initialize(std::weak_ptr<::Player>);

	/// <summary>
	/// UI全体の更新。入力デバイスの検知（キーボード/パッド）や、HP・アビリティのゲージ計算を行う。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// スプライトの再配置とタイマーのリセット。シーンのリトライ時などに呼び出す。
	/// </summary>
	void Reset();

	/// <summary>
	/// ポーズメニューの表示状態を設定
	/// </summary>
	/// <param name="pause">trueを指定するとゲーム画面を暗転させ、メニュー項目を表示する</param>
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
	/// HPバーの長さをプレイヤーの現在HP/最大HPの割合に応じてスケーリングする。
	/// </summary>
	void UpdateHPBar();

	/// <summary>
	/// ダッシュや必殺技のクールタイムを監視し、使用不可時のアイコンの暗転（シャドウ表示）を更新する。
	/// </summary>
	void UpdateAbilityCoolTime();

	/// <summary>
	/// ゲームパッド接続時のテクスチャの切り替え関数
	/// </summary>
	void UpdateControlUI();

private:

	uint32_t textureHandle_ = 0;
	uint32_t keyPauseTexture_ = 0;
	uint32_t padPauseTexture_ = 0;
	uint32_t keyBaseCtrlTexture_ = 0;
	uint32_t padBaseCtrlTexture_ = 0;
	uint32_t keySelectTexture_ = 0;
	uint32_t keyCheckSelectTexture_ = 0;
	uint32_t padSelectTexture_ = 0;
	uint32_t keyCtrlTexture_ = 0;
	uint32_t padCtrlTexture_ = 0;

	std::unique_ptr<KamataEngine::Sprite> hpBarSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> hpSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> hpBackSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> dashShadowSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> spShadowSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> ctrlSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> baseCtrlSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> pauseSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> selectSprite_[2];
	std::unique_ptr<KamataEngine::Sprite> selectCursorSprite_[2];
	std::unique_ptr<KamataEngine::Sprite> checkFrameSprite_ = nullptr;
	std::unique_ptr<KamataEngine::Sprite> checkSelectSprite_[2];
	std::unique_ptr<KamataEngine::Sprite> checkCursorSprite_[2];
	std::unique_ptr<KamataEngine::Sprite> selectKeySprite_[2];

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

	std::weak_ptr<::Player> player_;

	bool isPadConnected_ = false;
	XINPUT_STATE state_{};
};
}