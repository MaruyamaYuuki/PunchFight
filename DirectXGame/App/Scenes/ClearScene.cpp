#define NOMINMAX
#include <algorithm>
#include "ClearScene.h"
#include "../../Engine/Rendering/Fade.h"
#include "../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;

ClearScene::ClearScene() {}

ClearScene::~ClearScene() {}

void ClearScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cfg_ = GameConfigManager::GetInstance();

	camera_.Initialize();

	waitTimer_ = cfg_->getFloat("Scene.Clear.kInitialWaitTime");
	clearScaleSpeed_ = cfg_->getFloat("Scene.Clear.ClearText.kClearScaleSpeed");

	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelBoxFrame_ = Model::CreateFromOBJ("boxFrame", true);

	textureHandle_ = TextureManager::Load("clearScene/clearBack.png");
	backTexture_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	textureHandle_ = TextureManager::Load("clearScene/spotlight.png");
	spotlightTexture_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	textureHandle_ = TextureManager::Load("clearScene/frontShadow.png");
	frontShadowTexture_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	textureHandle_ = TextureManager::Load("clearScene/clearText.png");
	clearTextTexture_ = Sprite::Create(textureHandle_, {640.0f, 200.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	clearScale_ = 0.2f;
	clearTextTexture_->SetSize({0.0f, 0.0f}); // テクスチャサイズは拡大で調整するので0でOK
	textureHandle_ = TextureManager::Load("clearScene/keyGuide.png");
	pushSpaceTexture_ = Sprite::Create(textureHandle_, {640.0f, 360.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});

	player_ = new Player();
	player_->Initialize(modelPlayer_, modelBoxFrame_, modelBoxFrame_, cfg_->getVector3("Player.kClearInitialPos"));

	fade_ = new Fade();
	fade_->Initialize();
	fade_->SetAlpha(0.0f);
}

void ClearScene::Update() {
	switch (phase_) {
	case ClearScene::Phase::kWaite:
		UpdateWait();
		break;
	case ClearScene::Phase::kPlay:
		UpdatePlay();
		break;
	case ClearScene::Phase::kFadeOut:
		UpdateFadeOut();
		break;
	}
}

void ClearScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());
	backTexture_->Draw();
	if (isSpot_) {
		spotlightTexture_->Draw();
	}
	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();
	player_->Draw(camera_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());
	if (!isSpot_) {
    	frontShadowTexture_->Draw();
	} else {
		clearTextTexture_->Draw();
		if (pushSpaceShown_) {
    		pushSpaceTexture_->Draw();
		}
	}
	fade_->Draw();
	// 前景スプライト描画後処理
	Sprite::PostDraw();
}

void ClearScene::UpdateWait() { 
	waitTimer_ -= deltaTime_;
	player_->UpdateWorldTransform();
	if (waitTimer_ <= 0) {
		waitTimer_ = cfg_->getFloat("Scene.Clear.kSpotlightAppearWaitTime");
		phase_ = Phase::kPlay;
	}
}

void ClearScene::UpdatePlay() { 
	player_->ClearAnimation(isSpot_); 

	UpdateGoalWaite();
	UpdateClearText();
	UpdateInput();
}

void ClearScene::UpdateGoalWaite() {
	if (!player_->IsGoal() || player_->IsVictory())
		return;

	waitTimer_ -= deltaTime_;
	if (waitTimer_ <= 0.0f) {
		isSpot_ = true;
		waitTimer_ = cfg_->getFloat("Scene.Clear.kClearAnimeStartWaitTime");
	}
}

void ClearScene::UpdateClearText() {
	if (player_->IsVictory() && !clearStart_) {
		waitTimer_ -= deltaTime_;
		if (waitTimer_ <= 0.0f) {
			clearStart_ = true;
			clearScale_ = cfg_->getFloat("Scene.Clear.ClearText.kClearScaleStart");
			clearWaitTimer_ = cfg_->getFloat("Scene.Clear.kPushSpaceDisplayWaitTime");
		}
	}

	if (!clearStart_)
		return;

	const float kScaleEnd = cfg_->getFloat("Scene.Clear.ClearText.kClearScaleEnd");

	clearScale_ = std::min(clearScale_ + clearScaleSpeed_ * deltaTime_, kScaleEnd);

	const float baseW = cfg_->getFloat("Scene.Clear.ClearText.kClearTextBaseWidth");
	const float baseH = cfg_->getFloat("Scene.Clear.ClearText.kClearTextBaseHeight");
	clearTextTexture_->SetSize({baseW * clearScale_, baseH * clearScale_});

	if (clearScale_ >= kScaleEnd && clearWaitTimer_ > 0.0f) {
		clearWaitTimer_ -= deltaTime_;
		if (clearWaitTimer_ <= 0.0f) {
			pushSpaceShown_ = true;
		}
	}
}

void ClearScene::UpdateInput() {
	if (!pushSpaceShown_)
		return;

	if (input_->TriggerKey(DIK_E)) {
		fade_->Start(Fade::Status::AlphaFadeOut, cfg_->getFloat("Scene.Clear.kFadeOutDuration"));
		phase_ = Phase::kFadeOut;
	}
}

void ClearScene::UpdateFadeOut() {
	player_->UpdateWorldTransform();
	fade_->Update();

	if (fade_->IsFinished()) {
		isFinished_ = true;
	}
}
