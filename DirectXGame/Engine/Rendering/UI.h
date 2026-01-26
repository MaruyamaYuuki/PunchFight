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
	/// <returns></returns>
	void SetPause(bool pause) { isPaused_ = pause; }

	void SetPauseSelectIndex(int32_t index) { pauseSelectIndex_ = index; }

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

	KamataEngine::Vector2 dashIconSize_ = {64.0f, 64.0f};
	KamataEngine::Vector2 spIconSize = {64.0f, 64.0f};

	bool isPaused_ = false;
	int32_t pauseSelectIndex_ = 0;

	Player* player_ = nullptr;
};
}

