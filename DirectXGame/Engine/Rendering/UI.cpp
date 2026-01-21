#include "UI.h"
#include <cassert>
#include "../../App/Objects/Player.h"
#include "../Utility/GameConfigManager.h"

using namespace KamataEngine;

namespace MyEngine {
void UI::Initialize(::Player* player) {
	cfg_ = GameConfigManager::GetInstance();

	assert(player);
	player_ = player;

	dashIconSize_ = cfg_->getVector2("UI.kDashIconSize");
	spIconSize = cfg_->getVector2("UI.kSPIconSize");

	textureHandle_ = TextureManager::Load("UI/HPBar.png");
	hpBarSprite_ = Sprite::Create(textureHandle_, {5.0f, 5.0f});
	textureHandle_ = TextureManager::Load("UI/HP.png");
	hpSprite_ = Sprite::Create(textureHandle_, {9.0f, 5.0f});
	textureHandle_ = TextureManager::Load("UI/HPBack.png");
	hpBackSprite_ = Sprite::Create(textureHandle_, {5.0f, 5.0f});
	textureHandle_ = TextureManager::Load("UI/iconShadow.png");
	dashShadowSprite_ = Sprite::Create(textureHandle_, {62.0f, 58.0f});
	dashShadowSprite_->SetSize({64.0f, 0.0f});
	spShadowSprite_ = Sprite::Create(textureHandle_, {178.0f, 58.0f});
	spShadowSprite_->SetSize({64.0f, 0.0f});
	textureHandle_ = TextureManager::Load("UI/keyCTRL.png");
	keyCTRLSprite_ = Sprite::Create(textureHandle_, {5.0f, 5.0f});
	textureHandle_ = TextureManager::Load("UI/padCTRL.png");
	padCTRLSprite_ = Sprite::Create(textureHandle_, {5.0f, 5.0f});
	textureHandle_ = TextureManager::Load("UI/baseCTRLTest.png");
	ctrlSprite_ = Sprite::Create(textureHandle_, {5.0f, 130.0f});
}

void UI::Update() {
	UpdateHPBar();
	UpdateAbilityCoolTime();
}

void UI::Draw() {
	hpBackSprite_->Draw();
	hpSprite_->Draw();
	hpBarSprite_->Draw();
	dashShadowSprite_->Draw();
	spShadowSprite_->Draw();
	keyCTRLSprite_->Draw();
	ctrlSprite_->Draw();
}

void UI::Reset() {

	// === HPバー初期化 ===
	const float texWidth = 385.0f;
	const float texHeight = 122.0f;

	hpSprite_->SetTextureRect({0.0f, 0.0f}, {texWidth, texHeight});
	hpSprite_->SetSize({texWidth, texHeight});

	// === クールタイム影を消す ===
	dashShadowSprite_->SetSize({dashIconSize_.x, 0.0f});
	spShadowSprite_->SetSize({spIconSize.x, 0.0f});
}

void UI::UpdateHPBar() {

	// プレイヤーの HP 取得
	int hp = player_->GetHP(); // 0〜100など
	const int maxHP = 100;

	float hpRate = static_cast<float>(hp) / maxHP; // 0.0〜1.0

	// HPバーの元画像のサイズ
	const float texWidth = 385.0f;
	const float texHeight = 122.0f;

	// 描画する横幅を決定
	float drawWidth = texWidth * hpRate;

	// ==== SetTextureRect を使って描画範囲を指定 ====
	// 左から drawWidth 分だけ切り抜く
	hpSprite_->SetTextureRect(
	    {0.0f, 0.0f},          // テクスチャ左上
	    {drawWidth, texHeight} // 描画範囲（横幅だけ変わる）
	);

	// ==== 実際の描画サイズも合わせて変更 ====
	hpSprite_->SetSize({drawWidth, texHeight});
}

void UI::UpdateAbilityCoolTime() {

	// ============================
	//   ステップ クールタイム影
	// ============================
	float dashRate = 0.0f;

	if (!player_->CanStep()) {
		// 残クールタイム / 最大値
		dashRate = static_cast<float>(player_->GetStepCooldownTimer()) / player_->GetStepCooldownMax();
	} else {
		// クールタイム終了 → 完全に0
		dashRate = 0.0f;
	}

	float newDashHeight = dashIconSize_.y * dashRate;
	dashShadowSprite_->SetSize({dashIconSize_.x, newDashHeight});

	// ============================
	//  強攻撃クールタイムの影
	// ============================
	float spRate = 0.0f; // クールタイム 0 の時は 0%

	if (!player_->CanSpecialAttack()) {
		// クールタイム中 → 高さを割合に応じて戻していく
		spRate = player_->GetSPAttackCooldownTimer() / player_->GetSPAttackCooldownMax();
	}

	float newSPHeight = spIconSize.y * spRate;

	spShadowSprite_->SetSize({spIconSize.x, newSPHeight});
}
} // namespace MyEngine