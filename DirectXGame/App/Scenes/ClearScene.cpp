#define NOMINMAX
#include <algorithm>
#include "ClearScene.h"
#include "SceneManager.h"
#include "../../Engine/Rendering/Fade.h"
#include "../../Engine/Utility/GameConfigManager.h"

using namespace KamataEngine;
using MyEngine::GameConfigManager;
using MyEngine::Fade;

ClearScene::ClearScene() {}

ClearScene::~ClearScene() = default;

void ClearScene::Initialize() {
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	camera_.Initialize();

	waitTimer_ = GameConfigManager::GetInstance()->getFloat("Scene.Clear.kInitialWaitTime");
	clearScaleSpeed_ = GameConfigManager::GetInstance()->getFloat("Scene.Clear.ClearText.kClearScaleSpeed");

	modelPlayer_.reset(Model::CreateFromOBJ("quad", true));
	modelBoxFrame_.reset(Model::CreateFromOBJ("boxFrame", true));

	textureHandle_ = TextureManager::Load("clearScene/clearBack.png");
	backSprite_.reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}));
	textureHandle_ = TextureManager::Load("clearScene/clearText.png");
	clearSprite_.reset(Sprite::Create(textureHandle_, {640.0f, 200.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	clearScale_ = 0.2f;
	clearSprite_->SetSize({0.0f, 0.0f}); // テクスチャサイズは拡大で調整するので0でOK
	keyPushTexture_ = TextureManager::Load("clearScene/keyGuide.png");
	padPushTexture_ = TextureManager::Load("clearScene/padGuide.png");
	pushGuideSprite_.reset(Sprite::Create(keyPushTexture_, {640.0f, 360.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));

	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelBoxFrame_.get(), modelBoxFrame_.get());
	player_->SetTranslation(GameConfigManager::GetInstance()->getVector3("Player.kClearInitialPos"));
	player_->SetRotateX(GameConfigManager::GetInstance()->getFloat("Player.kClearSceneRotateX"));
	player_->SetMoveLimitEnabled(false);

	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->SetAlpha(0.0f);
}

void ClearScene::Update() {
	Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState_);

	isPadConnected_ = Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState_);

	isAButtonPressed_ = (state_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState_.Gamepad.wButtons & XINPUT_GAMEPAD_A);

	switch (phase_) {
	case ClearScene::Phase::kWait:
		UpdateWait();
		break;
	case ClearScene::Phase::kPlay:
		UpdatePlay();
		break;
	case ClearScene::Phase::kFadeOut:
		UpdateFadeOut();
		break;
	}

	if (isPadConnected_) {
		pushGuideSprite_->SetTextureHandle(padPushTexture_);
	} else {
		pushGuideSprite_->SetTextureHandle(keyPushTexture_);
	}
}

void ClearScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	backSprite_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	DirectXCommon::GetInstance()->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();
	if (phase_ != Phase::kWait) {
    	player_->Draw(camera_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	if (player_->IsVictory()) {
		clearSprite_->Draw();
		if (pushSpaceShown_) {
			pushGuideSprite_->Draw();
		}
	}
	fade_->Draw();
	// 前景スプライト描画後処理
	Sprite::PostDraw();
}

int ClearScene::GetNextScene() const {
	// 次はTitleSceneへ行くことをマネージャーに伝える
	return static_cast<int>(SceneManager::SceneType::kTitle);
}

void ClearScene::UpdateWait() { 
	waitTimer_ -= deltaTime_;
	player_->UpdateWorldTransform();
	if (waitTimer_ <= 0) {
		waitTimer_ = GameConfigManager::GetInstance()->getFloat("Scene.Clear.kPoseWaitTime");
		phase_ = Phase::kPlay;
	}
}

void ClearScene::UpdatePlay() { 
	player_->ClearAnimation(); 

	UpdateClearText();
	UpdateInput();
}

void ClearScene::UpdateClearText() {
	if (player_->IsVictory() && !clearStart_) {
		waitTimer_ -= deltaTime_;
		if (waitTimer_ <= 0.0f) {
			clearStart_ = true;
			clearScale_ = GameConfigManager::GetInstance()->getFloat("Scene.Clear.ClearText.kClearScaleStart");
			clearWaitTimer_ = GameConfigManager::GetInstance()->getFloat("Scene.Clear.kPushSpaceDisplayWaitTime");
		}
	}

	if (!clearStart_)
		return;

	const float kScaleEnd = GameConfigManager::GetInstance()->getFloat("Scene.Clear.ClearText.kClearScaleEnd");

	clearScale_ = std::min(clearScale_ + clearScaleSpeed_ * deltaTime_, kScaleEnd);

	const float baseW = GameConfigManager::GetInstance()->getFloat("Scene.Clear.ClearText.kClearTextBaseWidth");
	const float baseH = GameConfigManager::GetInstance()->getFloat("Scene.Clear.ClearText.kClearTextBaseHeight");
	clearSprite_->SetSize({baseW * clearScale_, baseH * clearScale_});

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

	if (input_->TriggerKey(DIK_E) || isAButtonPressed_) {
		fade_->Start(MyEngine::Fade::Status::AlphaFadeOut, GameConfigManager::GetInstance()->getFloat("Scene.Clear.kFadeOutDuration"));
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
