#pragma once
#include "KamataEngine.h"

/// <summary>
/// UI
/// </summary>
class Player;
class UI {
public:

	void Initialize(Player* player);

	void Update();

	void Draw();

private:

	void UpdateHPBar();

	void UpdateAbilityCoolTime();

private:
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* hpBarSprite_ = nullptr;
	KamataEngine::Sprite* hpSprite_ = nullptr;
	KamataEngine::Sprite* hpBackSprite_ = nullptr;
	KamataEngine::Sprite* dashShadowSprite_ = nullptr;
	KamataEngine::Sprite* spShadowSprite_ = nullptr;
	KamataEngine::Sprite* keyCTRLSprite_ = nullptr;
	KamataEngine::Sprite* padCTRLSprite_ = nullptr;
	KamataEngine::Sprite* ctrlSprite_ = nullptr;

	KamataEngine::Vector2 dashIconSize_ = {64.0f, 64.0f};
	KamataEngine::Vector2 spIconSize = {64.0f, 64.0f};

	Player* player_ = nullptr;
};
