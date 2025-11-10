#define NOMINMAX
#include "GameScene.h"
#include <chrono>
#include "../../Engine/Rendering/Fade.h"
#include "../../Engine/Math/Easing.h"
#include "../../Engine//Math/Collision.h"
#include <algorithm>

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
	modelBoxFrame_ = Model::CreateFromOBJ("boxFrame", true);

	textureHandle_ = TextureManager::Load("gameSelect.png");
	backTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 360.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	textureHandle_ = TextureManager::Load("readyText.png");
	readyTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	textureHandle_ = TextureManager::Load("fightText.png");
	fightTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	textureHandle_ = TextureManager::Load("knockDownText.png");
	gameOverTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 0}, {0.5f, 0.5f});
	textureHandle_ = TextureManager::Load("black.png");
	blackSprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1, 1, 1, 0});
	textureHandle_ = TextureManager::Load("resetText.png");
	resetTextSprite_ = Sprite::Create(textureHandle_, {640.0f, 500.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});

	startGongSEDataHandle_ = audio->LoadWave("audio/SE/startGong.wav");

	player_ = new Player();
	player_->Initialize(modelPlayer_,modelBoxFrame_);

	EnemyGenerate();

	stage_ = new StageManager();
	stage_->Initialize(1, 9);

	cameraController_ = new CameraController(); // 生成
	cameraController_->Initialize();            // 初期化
	cameraController_->SetTarget(player_);      // 追従対象をセット
	cameraController_->Reset();                 // リセット(瞬間合わせ)
	CameraController::Rect cameraArea = {0.0f, scrollArea[0], -8.0f, -1.0f};
	cameraController_->SetMovableArea(cameraArea);

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime_);

	prevTime_ = std::chrono::high_resolution_clock::now();
	waitTimer_ = 0.0f;
}

void GameScene::Update() {
	ChangePhase();
	GameOver();

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
		EnemyUpdate();
		AllCollision();
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
	enemyManager_->BackDraw(camera_, player_->GetWorldTransform().translation_);
	player_->Draw(camera_);
	enemyManager_->FrontDraw(camera_, player_->GetWorldTransform().translation_);
	//enemyManager_->Draw(camera_);

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

		if (player_->IsDead()) {
			blackSprite_->Draw();
			//gameOverTextSprite_->Draw();
		} 
		if (player_->GetHP() > 0) {
			//backTextSprite_->Draw();
		}
		if (isGameOverFallFinished_) {
			//resetTextSprite_->Draw();
		}
		break;
	case GameScene::Phase::kFadeOut:
		blackSprite_->Draw();
		gameOverTextSprite_->Draw();
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
			startTime_ = 1.5f;
			phase_ = Phase::kFight;
		}
		break;
	case GameScene::Phase::kFight:
		FightAnimation();
		if (fightTextAnimeFinished_) {
    		startTime_ -= deltaTime_;
    		if (startTime_ <= 0.0f) {
				audio->StopWave(startGongSEVoiceHandle_);
    			startTime_ = 4.0f;
    			phase_ = Phase::kPlay;
    		}
		}
		break;
	case GameScene::Phase::kPlay:
		if (input->TriggerKey(DIK_T)) {
			player_->TakeDamage(100);
		}
		break;
	case GameScene::Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			if (player_->IsDead()) {
    			ResetGame();
    			phase_ = Phase::kFadeIn;
    			fade_->Start(Fade::Status::FadeIn, fadeTime_);
			} else {
				isFinished_ = true;
			}
		}
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
		startGongSEVoiceHandle_ = audio->PlayWave(startGongSEDataHandle_, false, 1.0f);
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

void GameScene::GameOver() {
	if (player_->IsDead()) {
		// 経過時間を加算
		alphaCounter_ += 1.0f / 60.0f;
		if (alphaCounter_ > duration_)
			alphaCounter_ = duration_;

		// 正規化された進行度
		float t = alphaCounter_ / duration_;
		t = std::clamp(t, 0.0f, 1.0f);

		// イージングで滑らかフェード
		float easedT = Easing::EaseInQuad(t);

		// ===== 背景の黒フェード =====
		blackSprite_->SetColor({1, 1, 1, 0.8f * easedT});

		// ===== フェード完了後の「GAME OVER」落下 =====
		if (t >= 1.0f) {
			gameOverFallTimer_ += 1.0f / 60.0f;

			// イージングで自然な落下
			float fallT = std::min(gameOverFallTimer_ / gameOverFallDuration_, 1.0f);
			float easedFall = Easing::EaseOutBounce(fallT); // ポンっと落ちる感じ

			// Y位置を補間（上→中央）
			float startY = -200.0f;
			float endY = 300.0f;
			float currentY = startY + (endY - startY) * easedFall;

			gameOverTextSprite_->SetPosition({640.0f, currentY});
			gameOverTextSprite_->SetColor({1, 1, 1, 1.0f}); // 完全不透明

			if (fallT >= 1.0f && !isGameOverFallFinished_) {
				isGameOverFallFinished_ = true;
			}
		} else {
			// 落下前は画面外に置いておく
			gameOverTextSprite_->SetPosition({640.0f, -200.0f});
			gameOverTextSprite_->SetColor({1, 1, 1, 0.0f});
			gameOverFallTimer_ = 0.0f;
			isGameOverFallFinished_ = false;
		}

		// リトライ入力
		if (input->TriggerKey(DIK_R)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, fadeTime_);
		}
	}
}

void GameScene::ResetGame() {
	// === タイマー・フラグ初期化 ===
	alphaCounter_ = 0.0f;
	gameOverFallTimer_ = 0.0f;
	isGameOverFallFinished_ = false;

	// === プレイヤー関連 ===
	if (player_) {
		player_->Reset();
	}

	// === スプライトの初期状態 ===
	blackSprite_->SetColor({1, 1, 1, 0.0f});
	gameOverTextSprite_->SetColor({1, 1, 1, 0.0f});
	gameOverTextSprite_->SetPosition({640.0f, -200.0f});
}

void GameScene::EnemyGenerate() {
	enemyManager_ = new EnemyManager();
	enemyManager_->Initialize();

	// --- エリア追加（トリガー位置） ---
	enemyManager_->AddArea(10.0f);   // area 0
	enemyManager_->AddArea(25.0f);  // area 1
	enemyManager_->AddArea(55.0f); // area 2

	// --- 各エリアに敵を追加 ---
	// エリア0
	enemyManager_->AddSpawnToArea(0, EnemyType::Normal, {15, 1, 0});

	// エリア1
	enemyManager_->AddSpawnToArea(1, EnemyType::Normal, {30, 1, 0});

	// エリア2
	enemyManager_->AddSpawnToArea(2, EnemyType::Normal, {60, 1, 0});

}

void GameScene::EnemyUpdate() {
	// プレイヤー情報取得
	KamataEngine::Vector3 playerPos = player_->GetWorldTransform().translation_;


	// ----- 敵管理の更新 -----
	enemyManager_->Update(playerPos);

	// ----- エリアクリア判定 -----
	for (int i = 0; i < 3; i++) {
		if (enemyManager_->IsAreaCleared(i) && !areaClearedFlag_[i]) {
			areaClearedFlag_[i] = true;

			// ここにクリア時のイベント（必要なら）
			// 例：扉を開く、カメラロック解除
			// Door[i].Open();
		}
	}

    if (areaClearedFlag_[2]) {
		phase_ = Phase::kFadeOut;
		fade_->Start(Fade::Status::FadeOut, fadeTime_);
	} else if (areaClearedFlag_[1]) {
		CameraController::Rect area = {0.0f, scrollArea[2], -8.0f, -1.0f};
		cameraController_->SetMovableArea(area);
	} else if (areaClearedFlag_[0]) {
		CameraController::Rect area = {0.0f, scrollArea[1], -8.0f, -1.0f};
		cameraController_->SetMovableArea(area);
	}
}

void GameScene::AllCollision() {
	// ---プレイヤーの攻撃と敵の当たり判定---
	const HitBox& atk = player_->GetAttackHitBox();

	// 攻撃終了したら記録リセット
	if (!atk.active) {
		hitEnemiesThisAttack_.clear();
		return;
	}

	auto& enemies = enemyManager_->GetEnemies();

	for (auto& e : enemies) {
		// この攻撃中に当たってたらスキップ
		if (std::find(hitEnemiesThisAttack_.begin(), 
			          hitEnemiesThisAttack_.end(), 
			          e.get()) != hitEnemiesThisAttack_.end()) {
			continue;
		}

		if (Collision::AABB(atk, e->GetHitBox())) {
			// この攻撃中に当たった敵として記録
			hitEnemiesThisAttack_.push_back(e.get());

			Vector3 dir{player_->GetFacingDir(), 0, 0};
			e->OnHit(player_->GetAttackPower(), dir);
		}
	}
}
