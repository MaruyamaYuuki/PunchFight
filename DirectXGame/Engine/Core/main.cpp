#include <Windows.h>
#include "KamataEngine.h"
#include "../Utility/GameConfigManager.h"
#include "../../App/Scenes/SceneManager.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	// エンジンの初期化
	KamataEngine::Initialize(L"LE3C_16_マルヤマ_ユウキ_PunchFight");
	// DirectXCommonインスタンス
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	MyEngine::GameConfigManager::GetInstance()->Initialize();

	// シーンマネージャーの生成（これも unique_ptr 推奨）
	auto sceneManager = std::make_unique<SceneManager>();

	// 初期シーンの設定
	sceneManager->ChangeScene(SceneManager::SceneType::kTitle);

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		sceneManager->Update();

		// 描画処理
		dxCommon->PreDraw();

		sceneManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
