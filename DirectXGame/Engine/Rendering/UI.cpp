#include "UI.h"
#include <cassert>
#include "../../App/Objects/Player/Player.h"
#include "../Utility/GameConfigManager.h"

using namespace KamataEngine;

namespace MyEngine {
void UI::Initialize(std::weak_ptr<::Player> player) {

	player_ = player;

	dashIconSize_ = GameConfigManager::GetInstance()->getVector2("UI.kDashIconSize");
	spIconSize = GameConfigManager::GetInstance()->getVector2("UI.kSPIconSize");

	textureHandle_ = TextureManager::Load("UI/HPBar.png");
	hpBarSprite_.reset(Sprite::Create(textureHandle_, {5.0f, 5.0f}));
	textureHandle_ = TextureManager::Load("UI/HP.png");
	hpSprite_.reset(Sprite::Create(textureHandle_, {9.0f, 5.0f}));
	textureHandle_ = TextureManager::Load("UI/HPBack.png");
	hpBackSprite_.reset(Sprite::Create(textureHandle_, {5.0f, 5.0f}));
	textureHandle_ = TextureManager::Load("UI/iconShadow.png");
	dashShadowSprite_.reset(Sprite::Create(textureHandle_, {62.0f, 58.0f}));
	dashShadowSprite_->SetSize({64.0f, 0.0f});
	spShadowSprite_.reset(Sprite::Create(textureHandle_, {178.0f, 58.0f}));
	spShadowSprite_->SetSize({64.0f, 0.0f});
	keyCtrlTexture_ = TextureManager::Load("UI/keyCTRL.png");
	ctrlSprite_.reset(Sprite::Create(keyCtrlTexture_, {5.0f, 5.0f}));
	keyBaseCtrlTexture_ = TextureManager::Load("UI/baseCTRLTest.png");
	baseCtrlSprite_.reset(Sprite::Create(keyBaseCtrlTexture_, {5.0f, 130.0f}));
	keyPauseTexture_ = TextureManager::Load("UI/pause.png");
	pauseSprite_.reset(Sprite::Create(keyPauseTexture_, {0.0f, 0.0f}));
	textureHandle_ = TextureManager::Load("UI/selectRestart.png");
	selectSprite_[0].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f,0.5f}));
	textureHandle_ = TextureManager::Load("UI/selectToTitle.png");
	selectSprite_[1].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/cursor1.png");
	selectCursorSprite_[0].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/cursor2.png");
	selectCursorSprite_[1].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/checkFrame.png");
	checkFrameSprite_.reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/CheckSelectYes.png");
	checkSelectSprite_[0].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/CheckSelectNo.png");
	checkSelectSprite_[1].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/checkCursor1.png");
	checkCursorSprite_[0].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/checkCursor2.png");
	checkCursorSprite_[1].reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	keySelectTexture_ = TextureManager::Load("UI/selectWS.png");
	selectKeySprite_[0].reset(Sprite::Create(keySelectTexture_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));
	keyCheckSelectTexture_ = TextureManager::Load("UI/selectAD.png");
	selectKeySprite_[1].reset(Sprite::Create(keyCheckSelectTexture_, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}));

	padPauseTexture_ = TextureManager::Load("UI/padPause.png");
	padBaseCtrlTexture_ = TextureManager::Load("UI/padBaseCTRL.png");
	padSelectTexture_ = TextureManager::Load("UI/selectPad.png");
	padCtrlTexture_ = TextureManager::Load("UI/padCTRL.png");

	checkFrameSprite_->SetPosition(checkPos_);
	for (int i = 0; i < 2; i++) {
		selectSprite_[i]->SetPosition(selectPos_[i]);
		selectCursorSprite_[i]->SetPosition(selectPos_[i]);
		checkSelectSprite_[i]->SetPosition(checkPos_);
		checkCursorSprite_[i]->SetPosition(checkPos_);
		selectKeySprite_[i]->SetPosition({selectKeyPos_});
	}
}

void UI::Update() {
	isPadConnected_ = KamataEngine::Input::GetInstance()->GetJoystickState(0, state_);

	UpdateHPBar();
	UpdateAbilityCoolTime();

	UpdateControlUI();
}

void UI::Draw() {
	hpBackSprite_->Draw();
	hpSprite_->Draw();
	hpBarSprite_->Draw();
	dashShadowSprite_->Draw();
	spShadowSprite_->Draw();
	ctrlSprite_->Draw();
	baseCtrlSprite_->Draw();

	if (isPaused_) {
		pauseSprite_->Draw();
		for (int i = 0; i < 2; i++) {
			selectSprite_[i]->Draw();
			if (i == pauseSelectIndex_) {
				selectCursorSprite_[i]->Draw();
			}
		}

		if (isBackTitleChecked_) {
			checkFrameSprite_->Draw();
			checkSelectSprite_[0]->Draw();
		    checkSelectSprite_[1]->Draw();
			checkCursorSprite_[checkBackTitleIndex_]->Draw();
			selectKeySprite_[1]->Draw();
		} else {
			selectKeySprite_[0]->Draw();
		}
	}	

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
	auto p = player_.lock();
	int hp = p->GetHP(); // 0〜100など
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

		auto p = player_.lock();
	if (!p->CanStep()) {
		// 残クールタイム / 最大値
		dashRate = static_cast<float>(p->GetStepCooldownTimer()) / p->GetStepCooldownMax();
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

	if (!p->CanSpecialAttack()) {
		// クールタイム中 → 高さを割合に応じて戻していく
		spRate = p->GetSPAttackCooldownTimer() / p->GetSPAttackCooldownMax();
	}

	float newSPHeight = spIconSize.y * spRate;

	spShadowSprite_->SetSize({spIconSize.x, newSPHeight});
}
void UI::UpdateControlUI() {
	if (isPadConnected_) {
		// パッド用UI
		pauseSprite_->SetTextureHandle(padPauseTexture_);
		ctrlSprite_->SetTextureHandle(padCtrlTexture_);
		baseCtrlSprite_->SetTextureHandle(padBaseCtrlTexture_);

		selectKeySprite_[0]->SetTextureHandle(padSelectTexture_);
		selectKeySprite_[1]->SetTextureHandle(padSelectTexture_);
	} else {
		// キーボード用UI
		ctrlSprite_->SetTextureHandle(keyCtrlTexture_);
		baseCtrlSprite_->SetTextureHandle(keyBaseCtrlTexture_);

		selectKeySprite_[0]->SetTextureHandle(keySelectTexture_);
		selectKeySprite_[1]->SetTextureHandle(keyCheckSelectTexture_);
	}
}
} // namespace MyEngine