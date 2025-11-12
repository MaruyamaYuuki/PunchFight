#include <Windows.h>
#include "KamataEngine.h"
#include "../../App/Scenes/TitleScene.h"
#include  "../../App/Scenes/GameScene.h"

using namespace KamataEngine;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

/// <summary>
/// シーンの状態
/// </summary>
enum class Scene {
	kUnkown = 0,

	kTitle,
	kGame,
};
Scene scene = Scene::kUnkown;

/// <summary>
/// シーンの切り替え
/// </summary>
void ChangeScene();

/// <summary>
/// 各シーンの更新
/// </summary>
void UpdateScene();

/// <summary>
/// 各シーンの描画
/// </summary>
void DrawScene();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	// エンジンの初期化
	KamataEngine::Initialize(L"LE3C_16_マルヤマ_ユウキ_PunchFight");
	// DirectXCommonインスタンス
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

#ifdef _DEBUG
	gameScene = new GameScene();
	gameScene->Initialize();

	scene = Scene::kGame;
#else
	titleScene = new TitleScene();
	titleScene->Initialize();

	scene = Scene::kTitle;
#endif // DEBUG

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		UpdateScene();
		ChangeScene();

		// 描画処理
		dxCommon->PreDraw();

		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			scene = Scene::kGame;

			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle;

			delete gameScene;
			gameScene = nullptr;

			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}
