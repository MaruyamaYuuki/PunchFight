#pragma once
#include "KamataEngine.h"
#include "../../Engine/Math/WorldTransformEx.h"
#include "../../Engine/Rendering/Fade.h"
#include "../Objects/Stage.h"
#include "../Objects/Player.h"

class GameScene {
public:
	GameScene();

	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	bool IsFinished() { return isFinished_; }

private:
	KamataEngine::DirectXCommon* dxCommon = nullptr;
	KamataEngine::Input* input = nullptr;
	KamataEngine::Audio* audio = nullptr;

	KamataEngine::Camera camera_;
	WorldTransformEx worldTransform_;

	KamataEngine::Model* modelLoad_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* backTextSprite_ = nullptr;

	Stage* stage_ = nullptr;
	Player* player_ = nullptr;

	Fade* fade_ = nullptr;
	float fadeTime_ = 1.0;

	bool isFinished_ = false;
};
