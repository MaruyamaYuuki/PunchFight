#pragma once
#include "KamataEngine.h"

class Player;
namespace MyEngine {
/// <summary>
/// UI
/// </summary>
class GameConfigManager;
class UI {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="player">プレイヤー</param>
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

	/// <summary>
	/// ポーズ中かを取得
	/// </summary>
	void SetPause(bool pause) { isPaused_ = pause; }

	/// <summary>
	/// タイトルへ本当に戻るかのチェック中かを取得
	/// </summary>
	void SetBackTitleCheck(bool check) { isBackTitleChecked_ = check; }

	void SetPauseSelectIndex(int32_t index) { pauseSelectIndex_ = index; }

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

