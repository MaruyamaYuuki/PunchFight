#include "TitleScene.h"
#include "Easing.h"
#include <chrono>

using namespace KamataEngine;

TitleScene::TitleScene() {}

TitleScene::~TitleScene() { 
	delete titleSprite_; 
	delete startSprite_;
	for (int i = 0; i < 2; i++) {
		delete titleBackSprite[i];
	}
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
	for (int i = 0; i < 2; i++) {
		titleBackSprite[i] = Sprite::Create(textureHandle_, {bgPosX[i], 0.0f});
	}

	textureHandle_ = TextureManager::Load("punchFightTitle.png");
	titleSprite_ = Sprite::Create(textureHandle_, titlePos_, {1, 1, 1, 1}, {0.5f, 0.5f});
	titleSprite_->SetSize(titleSize_);

	textureHandle_ = TextureManager::Load("start.png");
	startSprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	startSprite_->SetPosition({640.0f, 500.0f});

	textureHandle_ = TextureManager::Load("black.png");
	fadeSprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	fadeSprite_->SetPosition({640.0f, 360.0f}); // 画面中心
	fadeSprite_->SetSize({1280.0f, 720.0f});    // 一応設定しておく

	textureHandle_ = TextureManager::Load("loadingText.png");
	loadingTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 250.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});

	textureHandle_ = TextureManager::Load("loadingBar.png");
	loadingBarSprite_ = Sprite::Create(textureHandle_, {430.0f, 300.0f});
	loadingBarSprite_->SetSize(loadingBarSize_);

	hitSEDataHandle_ = audio->LoadWave("audio/SE/hitSE.wav");
	doubleHitSEDataHandle_ = audio->LoadWave("audio/SE/doubleHitSE.wav");
	titleBGMDataHandle_ = audio->LoadWave("audio/BGM/titleBGM.wav");

	prevTime_ = std::chrono::high_resolution_clock::now();
	waitTimer_ = 0.0f;

}

void TitleScene::Update() { 
	Input::GetInstance()->GetJoystickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);
	isAButtonPressed = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A);

	for (int i = 0; i < 2; i++) {
		bgPosX[i] -= bgScrollSpeed_;
		if (bgPosX[i] <= -1367.0f) {
			bgPosX[i] = 1367.0f;
		}

		titleBackSprite[i]->SetPosition({bgPosX[i], 0.0f});
	}

	TitleAnimation();
	SpriteFlashUpdate();
	FadeOutUpdate();
	NowLoading();
}

void TitleScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	for (int i = 0; i < 2; i++) {
		titleBackSprite[i]->Draw();
	}

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

	if (!titleBlinking_ && !titleBlinkFinished_) {
    	startSprite_->Draw();
	}

	if (fadeOutStarted_) {
		fadeSprite_->Draw();
	}
	if (fadeOutFinished_) {
		loadingBarSprite_->Draw();
		loadingTextSprite_->Draw();
	}
	// 前景スプライト描画後処理
	Sprite::PostDraw();
}

void TitleScene::TitleAnimation() {
	// ---ゲーム開始時のアニメーション処理---
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
        hitSEVoiceHandle_ = audio->PlayWave(hitSEDataHandle_, false, 1.0f);
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
			audio->StopWave(hitSEVoiceHandle_);
		}
	}
	// -------------------------------------
}

void TitleScene::SpriteFlashUpdate() {
	if (titleAnimeFinished_) {
		if (!titleBlinking_ && !titleBlinkFinished_) {
			if (input->TriggerKey(DIK_E) || isAButtonPressed) {
				audio->StopWave(titleBGMVoiceHandle_);
				doubleHitSEVoiceHandle_ = audio->PlayWave(doubleHitSEDataHandle_, false, 1.0f);
				titleBlinking_ = true;
				blinkCount_ = 0;
				blinkTimer_ = 0.0f;
			}
		}

		// BGM を一度だけ再生
		if (!titleBGMStarted_) {
			titleBGMVoiceHandle_ = audio->PlayWave(titleBGMDataHandle_, true, 0.25f);
			titleBGMStarted_ = true;
		}
		// --- 点滅処理 ---
		if (titleBlinking_) {
			blinkTimer_ += 0.1f;

			// αをsin波で点滅（1周期で1回）
			float alpha = (std::sin(blinkTimer_ * 3.14159265f * 2.0f) + 1.0f) / 2.0f;
			titleSprite_->SetColor({1, 1, 1, alpha});

			// 一定周期を過ぎたら1回点滅完了
			if (blinkTimer_ >= 1.0f) {
				blinkTimer_ = 0.0f;
				blinkCount_++;

				if (blinkCount_ >= kMaxBlinkCount_) {
					titleBlinking_ = false;
					titleBlinkFinished_ = true;
					titleSprite_->SetColor({1, 1, 1, 1}); // 最後は表示状態に戻す
				}
			}
		}
		// ----------------

		else {
			// 通常の「PRESS START」点滅
			blinkTime_ += 0.03f;
			float alpha = (std::sin(blinkTime_ - 3.14159265f / 2.0f) + 1.0f) / 2.0f;
			startSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		}

	} else {
		startSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
	}
}

void TitleScene::FadeOutUpdate() {
	// --- フェードアウト処理 ---
	if (titleBlinkFinished_ && !fadeOutStarted_) {
		fadeWaitTime--;
		if (fadeWaitTime <= 0.0f) {
    		fadeOutStarted_ = true;
    		fadeScale_ = 0.0f;
    		fadeSprite_->SetColor({0, 0, 0, 1});  // 最初から不透明な黒
    		fadeSprite_->SetSize({10.0f, 10.0f}); // 点から始める
    		fadeSprite_->SetPosition({640.0f, 360.0f});
		}
	}

	if (fadeOutStarted_ && !fadeOutFinished_) {
		// 徐々に拡大
		fadeScale_ += 0.02f * fadeSpeed_;
		if (fadeScale_ > 1.5f)
			fadeScale_ = 1.5f;

		// サイズ更新（中央を基準に拡大）
		fadeSprite_->SetSize({1280.0f * fadeScale_, 720.0f * fadeScale_});

		// 拡大が十分なら終了
		if (fadeScale_ >= 1.5f) {
			fadeOutFinished_ = true;
		}
	}
}

void TitleScene::NowLoading() {
	// フェードアウト完了後にロード開始
	if (fadeOutFinished_ && !loadingStarted_) {
		loadingStarted_ = true;
		loadingBarSize_ = {0.0f, 40.0f};
		loadingBarSprite_->SetSize(loadingBarSize_);
	}

	// ロード中
	if (loadingStarted_ && !loadingFinished_) {
		loadingBarSize_.x += 1.5f;

		loadingBarSprite_->SetSize(loadingBarSize_);

		if (loadingBarSize_.x >= loadingXMaxSize_) {
			loadingBarSize_.x = loadingXMaxSize_;
			loadingFinished_ = true;
		}
	}
}
