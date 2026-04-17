#define NOMINMAX
#include "GameScene.h"
#include <chrono>
#include "SceneManager.h"
#include "../../Engine/Rendering/Fade.h"
#include "../../Engine/Math/Easing.h"
#include "../../Engine//Math/Collision.h"
#include "../../Engine/Utility/GameConfigManager.h"
#include <algorithm>

using namespace KamataEngine;
using MyEngine::GameConfigManager;
using MyEngine::CameraController;
using MyEngine::Fade;
using MyEngine::UI;

GameScene::GameScene() {}

GameScene::~GameScene() = default;

void GameScene::Initialize() { 
	dxCommon_ = DirectXCommon::GetInstance(); 
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	cfg_ = GameConfigManager::GetInstance();

	camera_.Initialize();

	worldTransform_.Initialize();

	// 基礎情報
	fadeTime_ = cfg_->getFloat("Global.kFadeTime");
	kInitialStartTime_ = cfg_->getFloat("Scene.Game.kReadyTime");
	moveLimit_ = cfg_->getFloatArray("Scene.Game.Area.kMoveLimitX_Area");
	scrollArea_ = cfg_->getFloatArray("Scene.Game.Area.kScrollAreaLimitX");
	cameraLimitZMin_ = cfg_->getFloat("Scene.Game.Area.kCameraLimitZMin");
	cameraLimitZMax_ = cfg_->getFloat("Scene.Game.Area.kCameraLimitZMax");
	stageNumber_ = cfg_->getInt("Scene.Game.StageDate.Stage1.kStageNumber");
	maxStageNumber_ = cfg_->getInt("Scene.Game.MaxStageNumber");
	stageRepeatCount_ = cfg_->getInt("Scene.Game.StageDate.Stage1.kRepeatCount");
	fightTextPos_ = cfg_->getVector2("Scene.Game.FightTextAnime.kFightTextCenterPos");
	fightTextSize_ = cfg_->getVector2("Scene.Game.FightTextAnime.kFightTextBaseSize");
	animeDuration_ = cfg_->getFloat("Scene.Game.FightTextAnime.kFightTextAnimeDuration");
	startScale_ = cfg_->getFloat("Scene.Game.FightTextAnime.kFightTextStartScale");
	waitDuration_ = cfg_->getFloat("Scene.Game.FightTextAnime.kFightTextWaitDuration");
	alphaDuration_ = cfg_->getFloat("Scene.Game.GameOver.kBlackSpriteAlphaDuration");
	gameOverFallDuration_ = cfg_->getFloat("Scene.Game.GameOver.kGameOverFallDuration");
	guideDuration_ = cfg_->getFloat("Scene.Game.Guide.kGuideDisplayDuration");
	blinkInterval_ = cfg_->getFloat("Scene.Game.Guide.kGuideBlinkInterval");
	maxBlinkCount_ = cfg_->getInt("Scene.Game.Guide.kGuideMaxBlinkCount");
	enemySpawnData_ = cfg_->getJsonArray("Scene.Game.EnemySpawn.Stage" + std::to_string(stageNumber_) + ".Areas");

	startTime_ = kInitialStartTime_;

	modelPlayer_.reset( Model::CreateFromOBJ("quad", true));
	modelSPAttack_.reset(Model::CreateFromOBJ("specialAttack", true));
	modelBoxFrame_.reset(Model::CreateFromOBJ("boxFrame", true));

	textureHandle_ = TextureManager::Load("readyText.png");
	readyTextSprite_.reset( Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("fightText.png");
	fightTextSprite_.reset(Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("knockDownText.png");
	gameOverTextSprite_.reset(Sprite::Create(textureHandle_, {640.0f, 300.0f}, {1, 1, 1, 0}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("black.png");
	blackSprite_.reset(Sprite::Create(textureHandle_, {0.0f, 0.0f}, {1, 1, 1, 0}));
	textureHandle_ = TextureManager::Load("scrollGuide.png");
	guideTexture_.reset(Sprite::Create(textureHandle_, {1100.0f, 450.0f},{1, 1, 1, 1}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("gameOverGuide.png");
	gameOverTextGuideSprite_.reset(Sprite::Create(textureHandle_, {640.0f, 400.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/gameOverSelectToRestart.png");
	gameOverSelectSprite_[0].reset(Sprite::Create(textureHandle_, {640.0f, 500.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	textureHandle_ = TextureManager::Load("UI/gameOverSelectToBackTitle.png");
	gameOverSelectSprite_[1].reset(Sprite::Create(textureHandle_, {640.0f, 500.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));

	startGongSEDataHandle_ = audio_->LoadWave("audio/SE/startGong.wav");
	bgmDataHandle_ = audio_->LoadWave("audio/BGM/gameBGM.wav");

	player_ = std::make_unique<Player>();
	player_->Initialize(modelPlayer_.get(), modelSPAttack_.get(), modelBoxFrame_.get());
	player_->SetEndMoveLimitX(moveLimit_[0]);

	EnemyGenerate();

	stage_ = std::make_unique<StageManager>();
	stage_->Initialize(stageNumber_, stageRepeatCount_);

	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_.get());
	cameraController_->Reset();                 // リセット(瞬間合わせ)
	CameraController::Rect cameraArea = {0.0f, scrollArea_[0], cameraLimitZMin_, cameraLimitZMax_};
	cameraController_->SetMovableArea(cameraArea);

	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime_);

	ui_ = std::make_unique<UI>();
	ui_->Initialize(player_.get());

	const size_t areaCount = std::size(scrollArea_);
	areaClearedFlag_.assign(areaCount, false);


	prevTime_ = std::chrono::high_resolution_clock::now();
	waitTimer_ = 0.0f;

	uiInput_ = std::make_unique<MyEngine::UIInputController>();

	isStartGongPlayed_ = true;
}

void GameScene::Update() {
	cmd = uiInput_->GetCommand();
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
		if (cmd.pause && !player_->IsDead()) {
			if (isPaused_) {
				ExitPause();
			} else {
				EnterPause();
			}
		}
		// ポーズ中はUI以外の更新を止める
		if (isPaused_) {
			UpdatePauseInput();
			ui_->Update();
			return;
		}
		if (input_->TriggerKey(DIK_L)) {
			player_->SetHP(0);
		}

		DebugText::GetInstance()->ConsolePrintf("Camera.Translate.x : %f\n\n", cameraController_->GetCamera().translation_.x);
		stage_->Update(cameraController_->GetCamera().translation_.x);
		player_->Update();
		if (input_->TriggerKey(DIK_B)) {

			isFinished_ = true;

		}

		ui_->Update();
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
	Sprite::PreDraw(dxCommon_->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	stage_->Draw(camera_);
	enemyManager_->BackDraw(camera_, player_->GetWorldTransform().translation_);
	player_->Draw(camera_);
	enemyManager_->FrontDraw(camera_, player_->GetWorldTransform().translation_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon_->GetCommandList());

	ui_->Draw();

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
		if (guideOn_) {
			// 半分より小さい時だけ表示する = 点滅
			if (guideTimer_ < blinkInterval_ * 0.5f) {
				guideTexture_->Draw();
			}
		}
		if (player_->IsDead()) {
			blackSprite_->Draw();
			gameOverTextSprite_->Draw();
		} 
		if (isGameOverFallFinished_) {
			if (gameOverSelectIndex_ == 0) {
				// リトライ強調
				gameOverSelectSprite_[0]->Draw();
			} else {
				// タイトル強調
				gameOverSelectSprite_[1]->Draw();
			}
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

int GameScene::GetNextScene() const {
	if (backToTitle_) {
		return static_cast<int>(SceneManager::SceneType::kTitle);
	}
	// 通常の終了（全ステージクリア等）ならクリアシーンへ
	return static_cast<int>(SceneManager::SceneType::kClear);
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			if (isStartGongPlayed_) {
    			phase_ = Phase::kReady;
			} else {
				phase_ = Phase::kPlay;
			}
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
				audio_->StopWave(startGongSEVoiceHandle_);
    			startTime_ = 4.0f;
    			phase_ = Phase::kPlay;
    			bgmVoiceHandle_ = audio_->PlayWave(bgmDataHandle_, true, 0.5f);
    		}
		}
		break;
	case GameScene::Phase::kPlay:
		if (IsAllAreaCleared() && player_->GetWorldTransform().translation_.x >= moveLimit_.back()) {
			audio_->StopWave(bgmVoiceHandle_);
			phase_ = Phase::kFadeOut;
			fade_->Start(MyEngine::Fade::Status::FadeOut, fadeTime_);
		}
		break;
	case GameScene::Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			if (player_->IsDead()) {
				if (backToTitle_) {
					isFinished_ = true;
				} else {
        			ResetGame();
        			phase_ = Phase::kFadeIn;
        			fade_->Start(Fade::Status::FadeIn, fadeTime_);
				}

			} else {
				GoToNextStage();
				ResetForNextStage();
				if (!isFinished_) {
					phase_ = Phase::kFadeIn;
					fade_->Start(Fade::Status::FadeIn, fadeTime_);
				}
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
		startGongSEVoiceHandle_ = audio_->PlayWave(startGongSEDataHandle_, false, 1.0f);
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

void GameScene::EnterPause() {
	isPaused_ = true;
	isBackTitleChecked_ = false;
	pauseSelectIndex_ = 0;
	checkBackTitleIndex_ = 1;

	ui_->SetPause(true);
	ui_->SetBackTitleCheck(false);
}

void GameScene::ExitPause() {
	isPaused_ = false;
	isBackTitleChecked_ = false;

	ui_->SetPause(false);
	ui_->SetBackTitleCheck(false);
}


void GameScene::UpdatePauseInput() {
	if (isBackTitleChecked_) {
		UpdateBackTitleCheckInput();
	} else {
		UpdatePauseMenuInput();
	}
}

void GameScene::UpdatePauseMenuInput() {

	if (cmd.move != 0) {
		pauseSelectIndex_ += cmd.move;
		pauseSelectIndex_ = std::clamp(pauseSelectIndex_, 0, 1);
	}

	if (cmd.decide) {

		switch (pauseSelectIndex_) {

		case 0: // 再開
			ExitPause();
			break;

		case 1: // タイトル
			isBackTitleChecked_ = true;
			checkBackTitleIndex_ = 1;
			ui_->SetBackTitleCheck(true);
			break;
		}
	}

	ui_->SetPauseSelectIndex(pauseSelectIndex_);
}

void GameScene::UpdateBackTitleCheckInput() {

	// 左右で Yes / No 切り替え
	if (cmd.moveLR != 0){
		checkBackTitleIndex_ = 1 - checkBackTitleIndex_;
	}

	if (cmd.decide) {
		if (checkBackTitleIndex_ == 0) {
			// Yes → タイトルへ
			backToTitle_ = true;
			audio_->StopWave(bgmVoiceHandle_);
			phase_ = Phase::kFadeOut;
			fade_->Start(MyEngine::Fade::Status::FadeOut, fadeTime_);
		} else {
			// No → ポーズに戻る
			isBackTitleChecked_ = false;
			ui_->SetBackTitleCheck(false);
		}
	}

	ui_->SetCheckBackTitleIndex(checkBackTitleIndex_);
}

void GameScene::GameOver() {
	if (player_->IsDead()) {
		audio_->StopWave(bgmVoiceHandle_);

		// 経過時間を加算
		alphaCounter_ += 1.0f / 60.0f;
		if (alphaCounter_ > alphaDuration_)
			alphaCounter_ = alphaDuration_;

		// 正規化された進行度
		float t = alphaCounter_ / alphaDuration_;
		t = std::clamp(t, 0.0f, 1.0f);

		// イージングで滑らかフェード
		float easedT = Easing::EaseInQuad(t);

		const float kMaxAlpha = cfg_->getFloat("Scene.Game.GameOver.kBlackSpriteMaxAlpha");

		// ===== 背景の黒フェード =====
		blackSprite_->SetColor({1, 1, 1, kMaxAlpha * easedT});

		// ===== フェード完了後の「GAME OVER」落下 =====
		if (t >= 1.0f) {
			gameOverFallTimer_ += 1.0f / 60.0f;

			// イージングで自然な落下
			float fallT = std::min(gameOverFallTimer_ / gameOverFallDuration_, 1.0f);
			float easedFall = Easing::EaseOutBounce(fallT); // ポンっと落ちる感じ

			// Y位置を補間（上→中央）
			const float kStartY = cfg_->getFloat("Scene.Game.GameOver.kGameOverTextStartY");
			const float kEndY = cfg_->getFloat("Scene.Game.GameOver.kGameOverTextEndY");
			float currentY = kStartY + (kEndY - kStartY) * easedFall;

			gameOverTextSprite_->SetPosition({640.0f, currentY});
			gameOverTextSprite_->SetColor({1, 1, 1, 1.0f}); // 完全不透明

			if (fallT >= 1.0f && !isGameOverFallFinished_) {
				isGameOverFallFinished_ = true;
				isGameOverSelect_ = true;
			}
		} else {
			// 落下前は画面外に置いておく
			gameOverTextSprite_->SetPosition({640.0f, -200.0f});
			gameOverTextSprite_->SetColor({1, 1, 1, 0.0f});
			gameOverFallTimer_ = 0.0f;
			isGameOverFallFinished_ = false;
		}

		// --- 選択入力 ---
		if (isGameOverSelect_) {

			// 上下で選択変更
			if (cmd.move != 0) {
				gameOverSelectIndex_ += cmd.move;
				gameOverSelectIndex_ = std::clamp(gameOverSelectIndex_, 0, 1);
			}

			// 決定
			if (cmd.decide) {

				if (gameOverSelectIndex_ == 0) {
					// リトライ
					backToTitle_ = false;
					isStartGongPlayed_ = true;
				} else {
					// タイトルへ
					backToTitle_ = true;
				}

				phase_ = Phase::kFadeOut;
				fade_->Start(Fade::Status::FadeOut, fadeTime_);
			}
		}
	}
}

void GameScene::ResetGame() {

	// フェーズ
	phase_ = Phase::kFadeIn;

	// タイマー・フラグ
	alphaCounter_ = 0.0f;
	gameOverFallTimer_ = 0.0f;
	isGameOverFallFinished_ = false;
	fightTextVisible_ = false;
	fightTextAnimeFinished_ = false;
	waitTimer_ = 0.0f;
	startTime_ = kInitialStartTime_;

	// プレイヤー
	player_->Reset();

	// 敵・エリア
	EnemyGenerate();
	areaClearedFlag_.assign(areaClearedFlag_.size(), false);
	guideOn_ = false;
	guideTimer_ = 0.0f;
	blinkCount_ = 0;

	// カメラ
	cameraController_->Reset();
	cameraController_->SetMovableArea({0.0f, scrollArea_[0], cameraLimitZMin_, cameraLimitZMax_});

	// UI
	ui_->Reset();
	isGameOverSelect_ = false;
	gameOverSelectIndex_ = 0;

	// スプライト
	blackSprite_->SetColor({1, 1, 1, 0});
	gameOverTextSprite_->SetColor({1, 1, 1, 0});
	gameOverTextSprite_->SetPosition({640.0f, -200.0f});

	// BGM
	audio_->StopWave(bgmVoiceHandle_);
}


void GameScene::EnemyGenerate() {
	//auto areas = cfg_->getJsonArray("Scene.Game.EnemySpawn.Areas");

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize();

	// --- 各エリアに敵を追加 ---
	for (size_t i = 0; i < enemySpawnData_.size(); ++i) {
		float triggerX = enemySpawnData_[i]["triggerX"].get<float>();
		enemyManager_->AddArea(triggerX);

		for (auto& e : enemySpawnData_[i]["enemies"]) {
			std::string typeStr = e["type"];
			auto posArr = e["pos"];
			Vector3 pos{
				posArr[0].get<float>(), 
				posArr[1].get<float>(), 
				posArr[2].get<float>()
			};

			EnemyType type = (typeStr == "Power") ? EnemyType::Power : EnemyType::Normal;

			enemyManager_->AddSpawnToArea(static_cast<int>(i), type, pos);
		}
	}
}

void GameScene::EnemyUpdate() {
	// プレイヤー情報取得
	KamataEngine::Vector3 playerPos = player_->GetWorldTransform().translation_;

	// ----- 敵管理の更新 -----
	enemyManager_->Update(playerPos);

	// ----- エリアクリア判定 -----
	for (size_t i = 0; i < areaClearedFlag_.size(); ++i) {
		if (enemyManager_->IsAreaCleared(static_cast<int32_t>(i)) && !areaClearedFlag_[i]) {

			areaClearedFlag_[i] = true;

			// ガイド矢印を表示させる
			guideOn_ = true;
			guideTimer_ = 0.0f;
			blinkCount_ = 0;

			cameraController_->StartReturnToPlayer();
		}
	}


    // ----- ガイド矢印の点滅処理 -----
	if (guideOn_) {
		guideTimer_ += deltaTime_;

		if (guideTimer_ >= blinkInterval_) {
			guideTimer_ = 0.0f;
			blinkCount_++;

			// 点滅終了
			if (blinkCount_ >= maxBlinkCount_) {
				guideOn_ = false;
			}
		}
	}

	// エリア進行処理（最も進んだエリア）
	for (int i = static_cast<int>(areaClearedFlag_.size()) - 1; i >= 0; --i) {
		if (!areaClearedFlag_[i])
			continue;

		// プレイヤー移動制限
		if (i + 1 < static_cast<int>(std::size(moveLimit_))) {
			player_->SetEndMoveLimitX(moveLimit_[i + 1]);
		}

		// カメラ制限（次のエリアのスクロール範囲を使う）
		if (i + 1 < static_cast<int>(std::size(scrollArea_))) {
			cameraController_->SetMovableArea({0.0f, scrollArea_[i + 1], cameraLimitZMin_, cameraLimitZMax_});
		}

		break;
	}
}

void GameScene::AllCollision() {

	// プレイヤーのヒットボックスを取得する
	const HitBox& pHitBox = player_->GetPlayerHitBox();

	// 敵リストの取得
	auto& enemies = enemyManager_->GetEnemies();

	#pragma region プレイヤーの通常攻撃と敵の当たり判定
	CheckPlayerAttackToEnemies(player_->GetAttackHitBox(), hitEnemiesThisAttack_, player_->GetAttackPower(), player_->GetFacingDir());
	#pragma endregion

	#pragma region プレイヤーの強攻撃と敵の当たり判定
	CheckPlayerAttackToEnemies(player_->GetSPAttackHitBox(), hitEnemiesThisSPAttack_, player_->GetSPAttackPower(), player_->GetSPAttackDir());
	#pragma endregion

	#pragma region 敵の攻撃とプレイヤーの当たり判定
	for (auto& e : enemies) {

		// 敵が攻撃状態ではないならスルー
		if (!e->IsAttackHitBoxActive()) {
			continue;
		}

		// 敵の攻撃ヒットボックス取得
		const HitBox& enemyAtk = e->GetAttackHitBox();

        // この攻撃でもうダメージを与えていたらスキップ
		if (e->HasDealtDamage()) {
			continue;
		}

		// AABB判定
		if (Collision::AABB(enemyAtk, pHitBox)) {

			// プレイヤーにダメージ処理
			player_->OnHit(e->GetAttackPower());

			// 1回の攻撃で多段ヒットしないよう enemy 側にフラグを付ける
			e->SetHasDealtDamage(true);
		}
	}
	#pragma endregion
}

void GameScene::CheckPlayerAttackToEnemies(const HitBox& attackHitBox, std::vector<EnemyBase*>& hitList, int attackPower, float attackDir) {
	auto& enemies = enemyManager_->GetEnemies();

	if (!attackHitBox.active) {
		hitList.clear();
		return;
	}

	for (auto& e : enemies) {

		// すでにこの攻撃で当たっている敵はスキップ
		if (std::find(hitList.begin(), hitList.end(), e.get()) != hitList.end()) {
			continue;
		}

		if (Collision::AABB(attackHitBox, e->GetHitBox())) {

			hitList.push_back(e.get());

			Vector3 dir{attackDir, 0, 0};
			e->OnHit(attackPower, dir);
		}
	}
}

bool GameScene::IsAllAreaCleared() const { 
	for (bool cleared : areaClearedFlag_) {
		if (!cleared) {
			return false;
		}
	}
	return true; 
}

void GameScene::GoToNextStage() { 
	stageNumber_++; 

	if (stageNumber_ > maxStageNumber_) {
		isFinished_ = true;
		return;
	}

	enemySpawnData_ = cfg_->getJsonArray("Scene.Game.EnemySpawn.Stage" + std::to_string(stageNumber_) + ".Areas");

	// ステージの再初期化
	stage_->Initialize(stageNumber_, stageRepeatCount_);
}

void GameScene::ResetForNextStage() {
	isStartGongPlayed_ = false;

	// プレイヤー位置
	player_->Reset();

	// 敵・エリア
	EnemyGenerate();
	areaClearedFlag_.assign(areaClearedFlag_.size(), false);
	guideOn_ = false;
	guideTimer_ = 0.0f;
	blinkCount_ = 0;

	// カメラ
	cameraController_->Reset();
	cameraController_->SetMovableArea({0.0f, scrollArea_[0], cameraLimitZMin_, cameraLimitZMax_});

	// UI
	ui_->Reset();
}
