#include "ClearScene.h"
#include "../../Engine/Rendering/Fade.h"

using namespace KamataEngine;

ClearScene::ClearScene() {}

ClearScene::~ClearScene() {}

void ClearScene::Initialize() {
	dxCommon = DirectXCommon::GetInstance();
	input = Input::GetInstance();
	audio = Audio::GetInstance();

	camera_.Initialize();
	// camera_.translation_ = {0.0f, 2.0f, -8.0f};

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
	player_->Initialize(modelPlayer_, modelBoxFrame_, modelBoxFrame_, Vector3{-5.0f, -1.1f, -45.0f});

	fade_ = new Fade();
	fade_->Initialize();
	fade_->SetAlpha(0.0f);
}

void ClearScene::Update() {
	switch (phase_) {
	case ClearScene::Phase::kWaite:
		waitTimer_ -= deltaTime_;
		if (waitTimer_ <= 0) {
			waitTimer_ = 2.0f;
			phase_ = Phase::kPlay;
		}
		player_->UpdateWorldTransform();
		break;
	case ClearScene::Phase::kPlay:
		player_->ClearAnimation(isSpot_);
		if (player_->IsGoal() && !player_->IsVictory()) {
			waitTimer_ -= deltaTime_;
			if (waitTimer_ <= 0) {
    			isSpot_ = true;
				waitTimer_ = 2.0f;
			}
		}

        if (player_->IsVictory() && !clearStart_) {
			waitTimer_ -= deltaTime_;
			if (waitTimer_ <= 0) {
				clearStart_ = true;
				clearScale_ = 0.0f;
				waitTimer_ = 1.0f;
			}
		}

        if (clearStart_) {
			clearScale_ += clearScaleSpeed_ * deltaTime_;
			if (clearScale_ > 1.0f) {
				clearScale_ = 1.0f;

				// 拡大完了後、待機タイマーを開始
				if (clearWaitTimer_ <= 0.0f) {
					clearWaitTimer_ = 2.0f; // 2秒待つ
				}
			}

			const float baseW = 696.0f;
			const float baseH = 144.0f;
			clearTextTexture_->SetSize({baseW * clearScale_, baseH * clearScale_});
		}

		// 拡大後の待機時間をカウント
		if (clearWaitTimer_ > 0.0f) {
			clearWaitTimer_ -= deltaTime_;
			if (clearWaitTimer_ <= 0.0f) {
				pushSpaceShown_ = true; // 待機後に表示する
			}
		}

		if (pushSpaceShown_) {
			if (input->TriggerKey(DIK_E)) {
				fade_->Start(Fade::Status::AlphaFadeOut, 1.5f);
				phase_ = Phase::kFadeOut;

			}
		}
		break;
	case ClearScene::Phase::kFadeOut:
		player_->UpdateWorldTransform();
		fade_->Update();
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		break;
	}
}

void ClearScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	backTexture_->Draw();
	if (isSpot_) {
		spotlightTexture_->Draw();
	}
	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();
	player_->Draw(camera_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
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
