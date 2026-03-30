#include "TitleScene.h"
#include "SceneManager.h"
#include "../../Engine/Math/Easing.h"
#include "../../Engine/Rendering/Fade.h"
#include "../../Engine/Utility/GameConfigManager.h"
#include <chrono>

using namespace KamataEngine;
using MyEngine::GameConfigManager;
using MyEngine::Fade;

TitleScene::TitleScene() {}

TitleScene::~TitleScene() = default;

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cfg_ = GameConfigManager::GetInstance();

	camera_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.rotation_.x -= 0.3f;
	worldTransform_.translation_.y -= 5.0f;

	
	fadeTime_ = cfg_->getFloat("Global.kFadeTime");

	waitDuration_ = cfg_->getFloat("Scene.Title.kTitleWaitDuration");
	animeDuration_ = cfg_->getFloat("Scene.Title.Anime.kTitleAnimeDuration");
	startScale_ = cfg_->getFloat("Scene.Title.Anime.kTitleAnimeStartScale");
	animeEndSize_ = cfg_->getFloat("Scene.Title.Anime.kTitleAnimeEndScale");
	bgmVolume_ = cfg_->getFloat("Scene.Title.Anime.kTitleBGMVolume");
	seVolume_ = cfg_->getFloat("Scene.Title.Anime.kTitleSEVolume");
	maxBlinkCount_ = cfg_->getInt("Scene.Title.Anime.kMaxBlinkCount");
	
	bgScrollSpeed_ = cfg_->getFloat("Scene.Title.Background.kBGScrollSpeed");
	bgResetPosX_ = cfg_->getFloat("Scene.Title.Background.kBGResetPosX");

	titlePos_ = cfg_->getVector2("Scene.Title.Sprites.kTitleSpriteCenterPos");
	titleSize_ = cfg_->getVector2("Scene.Title.Sprites.kTitleSpriteBaseSize");

	

	textureHandle_ = TextureManager::Load("brickWall.png");
	titleBackSprite_[0].reset(Sprite::Create(textureHandle_, {bgPosX_[0], 0.0f}));
	titleBackSprite_[1].reset(Sprite::Create(textureHandle_, {bgPosX_[1], 0.0f}));

	textureHandle_ = TextureManager::Load("punchFightTitle.png");
	titleSprite_.reset(Sprite::Create(textureHandle_, titlePos_, {1, 1, 1, 1}, {0.5f, 0.5f}));
	titleSprite_->SetSize(titleSize_);

	keyTexture_ = TextureManager::Load("eStart.png");
	padTexture_ = TextureManager::Load("padStart.png");
	startSprite_.reset(Sprite::Create(keyTexture_, {0.0f, 0.0f}, {1, 1, 1, 0}, {0.5f, 0.5f}));
	startSprite_->SetPosition({640.0f, 500.0f});

	hitSEDataHandle_ = audio_->LoadWave("audio/SE/hitSE.wav");
	doubleHitSEDataHandle_ = audio_->LoadWave("audio/SE/doubleHitSE.wav");
	titleBGMDataHandle_ = audio_->LoadWave("audio/BGM/titleBGM.wav");

	prevTime_ = std::chrono::high_resolution_clock::now();
	waitTimer_ = 0.0f;

	baseStartSize_ = startSprite_->GetSize();

	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(MyEngine::Fade::Status::FadeOut, fadeTime_);
}

void TitleScene::Update() { 
	Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState_);

	isPadConnected_ = Input::GetInstance()->GetJoystickState(0, state_);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState_);

	isBButtonPressed_ = (state_.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState_.Gamepad.wButtons & XINPUT_GAMEPAD_B);

	for (int32_t i = 0; i < 2; i++) {
		bgPosX_[i] -= bgScrollSpeed_;
		if (bgPosX_[i] <= -bgResetPosX_) {
			bgPosX_[i] = bgResetPosX_;
		}

		titleBackSprite_[0]->SetPosition({bgPosX_[0], 0.0f});
		titleBackSprite_[1]->SetPosition({bgPosX_[1], 0.0f});

		// 背景の明るさをサイン波で変える (0.8f〜1.0fの間で変化)
		float bgBrightness = 0.9f + std::sin(floatingTimer_ * 0.5f) * 0.1f;
		titleBackSprite_[0]->SetColor({bgBrightness, bgBrightness, bgBrightness, 1.0f});
		titleBackSprite_[1]->SetColor({bgBrightness, bgBrightness, bgBrightness, 1.0f});
	}

	TitleAnimation();
	SpriteFlashUpdate();
	if (titleBlinkFinished_) {
		fade_->Update();
	}
	if (fade_->IsFinished()) {
		fade_->Stop();
		isFinished_ = true;
	}

	if (isPadConnected_) {
		startSprite_->SetTextureHandle(padTexture_);
	} else {
		startSprite_->SetTextureHandle(keyTexture_);
	}
}

void TitleScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());

	titleBackSprite_[0]->Draw();
	titleBackSprite_[1]->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());

	if (titleVisible_) {
    	titleSprite_->Draw();
	}

	if (!titleBlinking_ && !titleBlinkFinished_) {
    	startSprite_->Draw();
	}

	if (titleBlinkFinished_) {
    	fade_->Draw();
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
        hitSEVoiceHandle_ = audio_->PlayWave(hitSEDataHandle_, false, seVolume_);
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
			audio_->StopWave(hitSEVoiceHandle_);
		}
	}
	// -------------------------------------

	// タイトルアニメーション終了後、常に実行
	if (titleAnimeFinished_) {
		// 時間経過でsin波を作る (0.05fは速度、2.0fは揺れ幅)
		floatingTimer_ += 0.05f;
		float offset = std::sin(floatingTimer_) * 5.0f;

		// 元の座標(titlePos_)にオフセットを加える
		titleSprite_->SetPosition({titlePos_.x, titlePos_.y + offset});
	}
}

void TitleScene::SpriteFlashUpdate() {
	if (titleAnimeFinished_) {
		if (!titleBlinking_ && !titleBlinkFinished_) {
			if (input_->TriggerKey(DIK_E) || isBButtonPressed_) {
				audio_->StopWave(titleBGMVoiceHandle_);
				doubleHitSEVoiceHandle_ = audio_->PlayWave(doubleHitSEDataHandle_, false, seVolume_);
				titleBlinking_ = true;
				blinkCount_ = 0;
				blinkTimer_ = 0.0f;
			}
		}

		// BGM を一度だけ再生
		if (!titleBGMStarted_) {
			titleBGMVoiceHandle_ = audio_->PlayWave(titleBGMDataHandle_, true, bgmVolume_);
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

				if (blinkCount_ >= maxBlinkCount_) {
					titleBlinking_ = false;
					titleBlinkFinished_ = true;
					titleSprite_->SetColor({1, 1, 1, 1}); // 最後は表示状態に戻す
				}
			}
		}
		// ----------------

		else {
			// 通常の「PRESS START」演出
			blinkTime_ += 0.03f;

			// 1. サイン波の基準値を計算 (共通の波形を使用する)
			// sinの結果は -1.0 ～ 1.0 なので、+1.0して 0.0 ～ 2.0 に変換
			float sinValue = std::sin(blinkTime_);

			// 2. アルファ値の計算 (0.0 ～ 1.0)
			// sinValue が 0.0（元のsinが-1.0）のとき、透明になる
			float alpha = (sinValue + 1.0f) / 2.0f;

			// 3. 鼓動演出の計算 (0.95 ～ 1.05)
			// sinValue が 0.0（最小）のとき、倍率は 1.0 + (-0.05) = 0.95 となる
			float beat = 1.0f + (sinValue * 0.05f);

			// サイズと色を適用
			startSprite_->SetSize({baseStartSize_.x * beat, baseStartSize_.y * beat});
			startSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
		}

	} else {
		startSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.0f});
	}
}

int TitleScene::GetNextScene() const {
	// 次はGameSceneへ行くことをマネージャーに伝える
	return static_cast<int>(SceneManager::SceneType::kGame);
}
