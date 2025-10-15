#include "GameScene.h"

using namespace KamataEngine;

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelLoad_;

	delete stage_;
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

	stage_ = new Stage();
	stage_->Initialize(modelLoad_);

	player_ = new Player();
	player_->Initialize(modelPlayer_);

	cameraController_ = new CameraController(); // 生成
	cameraController_->Initialize();            // 初期化
	cameraController_->SetTarget(player_);      // 追従対象をセット
	cameraController_->Reset();                 // リセット(瞬間合わせ)
	CameraController::Rect cameraArea = {0.0f, 100 - 12.0f, -8.0f,-1.0f};
	cameraController_->SetMovableArea(cameraArea);

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime_);
}

void GameScene::Update() {
	stage_->Update();
	player_->Update();
	fade_->Update(); 

	if (input->TriggerKey(DIK_B)) {
		isFinished_ = true;
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

	fade_->Draw();

	// 前景スプライト描画後処理
	Sprite::PostDraw();
}