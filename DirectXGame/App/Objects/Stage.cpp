#include "Stage.h"
#include <cassert>

using namespace KamataEngine;

void Stage::Initialize(Model* model) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
}

void Stage::Update() { worldTransform_.UpdateMatrix(); }

void Stage::Draw(Camera& camera) { 
	model_->Draw(worldTransform_, camera); 
}

