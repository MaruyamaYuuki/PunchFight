#include "TitleScene.h"
#include "Easing.h"
#include <chrono>

using namespace KamataEngine;

TitleScene::TitleScene() {}

TitleScene::~TitleScene() { 
	delete titleSprite_; 
	audio->StopWave(titleBGMVoiceHandle_);
}

void TitleScene::Initialize() {
	dxCommon = DirectXCommon::GetInstance();
	input = Input::GetInstance();
	audio = Audio::GetInstance();

	camera_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.rotation_.x -= 0.3f;
	worldTransform_.translation_.y -= 5.0f;

	textureHandle_ = TextureManager::Load("brickWall.png");
	titleBackSprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});

	textureHandle_ = TextureManager::Load("punchFightTitle.png");
	titleSprite_ = Sprite::Create(textureHandle_, titlePos_, {1, 1, 1, 1}, {0.5f, 0.5f});
	titleSprite_->SetSize(titleSize_);

	textureHandle_ = TextureManager::Load("start.png");
	startSprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	startSprite_->SetPosition({640.0f, 600.0f});

	punchSEDataHandle_ = audio->LoadWave("punchSE.wav");
	titleBGMDataHandle_ = audio->LoadWave("titleBGM.wav");

	prevTime_ = std::chrono::high_resolution_clock::now();
	waitTimer_ = 0.0f;

}

void TitleScene::Update() { 
	TitleAnimation();

	if (titleAnimeFinished_) {
		// BGM を一度だけ再生
		if (!titleBGMStarted_) {
			titleBGMVoiceHandle_ = audio->PlayWave(titleBGMDataHandle_, true, 0.25f);
			titleBGMStarted_ = true;
		}
		blinkTime_ += 0.03f;
		float alpha = (std::sin(blinkTime_ - 3.14159265f / 2.0f) + 1.0f) / 2.0f;
		startSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	} else {
		startSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
	}

}

void TitleScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	titleBackSprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	if (titleVisible_) {
    	titleSprite_->Draw();
	}

	startSprite_->Draw();

	// 前景スプライト描画後処理
	Sprite::PostDraw();
}

void TitleScene::TitleAnimation() {
	// deltaTime を実フレーム時間で計算
	auto now = std::chrono::high_resolution_clock::now();
	float deltaTime = duration_cast<std::chrono::duration<float>>(now - prevTime_).count();
	prevTime_ = now;

	// 安全対策：極端に大きい時間は潰す
	if (deltaTime > 0.1f)
		deltaTime = 0.1f;

	// ★ 待ち時間処理
	waitTimer_ += deltaTime;
	if (waitTimer_ < waitDuration_) {
		return; // まだ出さない
	}

	// 初回だけ表示開始
	if (!titleVisible_) {
		titleVisible_ = true;
		titleAnimeTimer_ = 0.0f;
		titleSprite_->SetSize({titleSize_.x * startScale_, titleSize_.y * startScale_});
        punchSEVoiceHandle_ = audio->PlayWave(punchSEDataHandle_, false, 1.0f);
	}

	if (titleVisible_) {
		// アニメーションタイマー更新
		titleAnimeTimer_ += deltaTime;
		float t = titleAnimeTimer_ / animeDuration_;
		if (t > 1.0f)
			t = 1.0f;

		// EaseOutBack を使ってポンっと置く
		float eased = Easing::EaseOutBack(t);
		float scale = startScale_ + (1.0f - startScale_) * eased;

		Vector2 size = {titleSize_.x * scale, titleSize_.y * scale};
		titleSprite_->SetSize(size);

		

		// 終了判定
		if (t >= 1.0f) {
			// 完全にアニメ終了
			titleAnimeFinished_ = true;
			audio->StopWave(punchSEVoiceHandle_);
		}
	}
}
