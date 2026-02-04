#include <Windows.h>
#include "KamataEngine.h"
#include "../Utility/GameConfigManager.h"
#include "../../App/Scenes/TitleScene.h"
#include  "../../App/Scenes/GameScene.h"
#include "../../App/Scenes/ClearScene.h"

using namespace KamataEngine;

std::unique_ptr<TitleScene> titleScene;
std::unique_ptr<GameScene> gameScene;
std::unique_ptr<ClearScene> clearScene;

/// <summary>
/// シーンの状態
/// </summary>
enum class Scene {
	kUnkown = 0,

	kTitle,
	kGame,
	kClear,
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

	MyEngine::GameConfigManager::GetInstance()->Initialize();

#ifdef _DEBUG
	// std::make_unique を使用して生成
	gameScene = std::make_unique<GameScene>();
	gameScene->Initialize();
	scene = Scene::kGame;
#else
	titleScene = std::make_unique<TitleScene>();
	titleScene->Initialize();
	scene = Scene::kTitle;
#endif

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
		if (titleScene && titleScene->IsFinished()) {
			scene = Scene::kGame;
			titleScene.reset();
			gameScene = std::make_unique<GameScene>();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene && gameScene->IsFinished()) {
			if (gameScene->IsBackToTitle()) {
				scene = Scene::kTitle;
				gameScene.reset();
				titleScene = std::make_unique<TitleScene>();
				titleScene->Initialize();
			} else {
				scene = Scene::kClear;
				gameScene.reset();
				clearScene = std::make_unique<ClearScene>();
				clearScene->Initialize();
			}
		}
		break;
	case Scene::kClear:
		if (clearScene && clearScene->IsFinished()) {
			scene = Scene::kTitle;
			clearScene.reset();
			titleScene = std::make_unique<TitleScene>();
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
	case Scene::kClear:
		clearScene->Update();
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
	case Scene::kClear:
		clearScene->Draw();
		break;
	}
}
