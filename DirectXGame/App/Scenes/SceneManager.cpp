#include "SceneManager.h"
#include "TitleScene.h"
#include  "GameScene.h"
#include "ClearScene.h"

void SceneManager::Update() {
	if (!currentScene_)
		return;

	// 現在のシーンを更新
	currentScene_->Update();

	// シーンが終了していたら、次のシーンへ切り替え
	if (currentScene_->IsFinished()) {
		// GetNextScene() を使って列挙型にキャスト
		ChangeScene(static_cast<SceneType>(currentScene_->GetNextScene()));
	}
}

void SceneManager::Draw() {
	if (currentScene_) {
		currentScene_->Draw();
	}
}

void SceneManager::ChangeScene(SceneType nextScene) {
	// 現在のシーンを破棄（unique_ptr なので reset でOK）
	currentScene_.reset();

	// 次のシーンを生成
	switch (nextScene) {
	case SceneType::kTitle:
		currentScene_ = std::make_unique<TitleScene>();
		break;
	case SceneType::kGame:
		currentScene_ = std::make_unique<GameScene>();
		break;
	case SceneType::kClear:
		currentScene_ = std::make_unique<ClearScene>();
		break;
	}

	// 生成したシーンを初期化
	if (currentScene_) {
		currentScene_->Initialize();
	}
}