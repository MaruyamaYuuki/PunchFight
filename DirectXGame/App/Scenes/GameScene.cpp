#include "GameScene.h"
#include <chrono>
#include "../../Engine/Rendering/Fade.h"
#include "../../Engine/Math/Easing.h"

using namespace KamataEngine;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelLoad_;
	delete modelPlayer_;

	delete player_;
	delete stage_;
	delete cameraController_;
	delete fade_;
}

void GameScene::Initialize() { 
	dxCommon = DirectXCommon::GetInstance(); 
	input = Input::GetInstance();
	audio = Audio::GetInstance();

	camera_.Initialize();
	//camera_.translation_ = {0.0f, 2.0f, -8.0f};

	worldTransform_.Initialize();

	modelLoad_ = Model::CreateFromOBJ("load", true);
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	textureHandle_ = TextureManager::Load("BackTitle.png");
	backTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 360.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	textureHandle_ = TextureManager::Load("readyText.png");
	readyTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	textureHandle_ = TextureManager::Load("fightText.png");
	fightTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});

	player_ = new Player();
	player_->Initialize(modelPlayer_);

	stage_ = new StageManager();
	stage_->Initialize(1, 9);

	cameraController_ = new CameraController(); // 生成
	cameraController_->Initialize();            // 初期化
	cameraController_->SetTarget(player_);      // 追従対象をセット
	cameraController_->Reset();                 // リセット(瞬間合わせ)
	CameraController::Rect cameraArea = { -100.0f, 100 - 5.0f, -8.0f, -1.0f};
	cameraController_->SetMovableArea(cameraArea);

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime_);

	prevTime_ = std::chrono::high_resolution_clock::now();
	waitTimer_ = 0.0f;
}

void GameScene::Update() {
	ChangePhase();

	switch (phase_) {
	case GameScene::Phase::kFadeIn:
	case GameScene::Phase::kReady:
	case GameScene::Phase::kFight:
		stage_->Update(cameraController_->GetCamera().translation_.x);
		player_->UpdateWorldTransform();
		break;
	case GameScene::Phase::kPlay:
		DebugText::GetInstance()->ConsolePrintf("Camera.Translate.x : %f\n\n", cameraController_->GetCamera().translation_.x);
		stage_->Update(cameraController_->GetCamera().translation_.x);
		player_->Update();
		if (input->TriggerKey(DIK_B)) {

			isFinished_ = true;

		}
		break;
	case GameScene::Phase::kFadeOut:
		break;
	default:
		break;
	}

	cameraController_->Update();

	const Camera& cameraViewProjection = cameraController_->GetCamera();
	camera_.matView = cameraViewProjection.matView;
	camera_.matProjection = cameraViewProjection.matProjection;
	camera_.TransferMatrix();
}

void GameScene::Draw() {
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	stage_->Draw(camera_);
	player_->Draw(camera_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	//backTextSprite_->Draw();


	switch (phase_) {
	case GameScene::Phase::kFadeIn:
    	fade_->Draw();
		break;
	case GameScene::Phase::kReady:
		if (startTime_ <= 2.0f) {
    		readyTextSprite_->Draw();
		}
		break;
	case GameScene::Phase::kFight:
		if (fightTextVisible_) {
    		fightTextSprite_->Draw();
		}
		break;
	case GameScene::Phase::kPlay:
		backTextSprite_->Draw();
		break;
	case GameScene::Phase::kFadeOut:
		fade_->Draw();
		break;
	}

	// 前景スプライト描画後処理
	Sprite::PostDraw();
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kReady;
		}
		break;
	case GameScene::Phase::kReady:
		startTime_ -= deltaTime_;
		if (startTime_ <= 0.0f) {
			startTime_ = 1.0f;
			phase_ = Phase::kFight;
		}
		break;
	case GameScene::Phase::kFight:
		FightAnimation();
		if (fightTextAnimeFinished_) {
    		startTime_ -= deltaTime_;
    		if (startTime_ <= 0.0f) {

    			startTime_ = 4.0f;
    			phase_ = Phase::kPlay;
    		}
		}
		break;
	case GameScene::Phase::kPlay:
		break;
	case GameScene::Phase::kFadeOut:
		break;
	default:
		break;
	}
}

void GameScene::FightAnimation() {
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
	if (!fightTextVisible_) {
		fightTextVisible_ = true;
		fightTextAnimeTimer_ = 0.0f;
		fightTextSprite_->SetSize({fightTextSize_.x * startScale_, fightTextSize_.y * startScale_});
	}

	if (fightTextVisible_) {
		// アニメーションタイマー更新
		fightTextAnimeTimer_ += deltaTime;
		float t = fightTextAnimeTimer_ / animeDuration_;
		if (t > 1.0f)
			t = 1.0f;

		// EaseOutBack を使ってポンっと置く
		float eased = Easing::EaseOutBack(t);
		float scale = startScale_ + (1.0f - startScale_) * eased;

		Vector2 size = {fightTextSize_.x * scale, fightTextSize_.y * scale};
		fightTextSprite_->SetSize(size);

		// 終了判定
		if (t >= 1.0f) {
			// 完全にアニメ終了
			fightTextAnimeFinished_ = true;
		}
	}
	// -------------------------------------
}
