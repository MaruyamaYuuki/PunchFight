#include "Fade.h"
#define NOMINMAX
#include <algorithm>

using namespace KamataEngine;

Fade::~Fade() { delete sprite_; }

void Fade::Initialize() {
	textureHandle_ = TextureManager::Load("black.png");
	// 中心基準で拡大縮小するためアンカーを(0.5, 0.5)に設定
	sprite_ = Sprite::Create(textureHandle_, {640.0f, 360.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
	sprite_->SetColor({0, 0, 0, 1});
	sprite_->SetSize({1280.0f, 720.0f});
	scale_ = 0.0f;
}

void Fade::Update() {
	if (status_ == Status::None) {
		return;
	}

	// 経過時間加算
	counter_ += 1.0f / 60.0f;
	if (counter_ >= duration_) {
		counter_ = duration_;
	}
	float t = counter_ / duration_;

	// フェード状態による分岐
	switch (status_) {
	case Fade::Status::FadeIn:
		// --- フェードイン（中央の黒が縮んで消える）---
		scale_ = std::lerp(1.5f, 0.0f, t);
		sprite_->SetSize({1280.0f * scale_, 720.0f * scale_});
		sprite_->SetColor({0, 0, 0, 1});
		sprite_->SetPosition({640.0f, 360.0f});
		break;

	case Fade::Status::FadeOut:
		// --- フェードアウト（中央から黒が広がる）---
		scale_ = std::lerp(0.0f, 1.5f, t);
		sprite_->SetSize({1280.0f * scale_, 720.0f * scale_});
		sprite_->SetColor({0, 0, 0, 1});
		sprite_->SetPosition({640.0f, 360.0f});
		break;

	default:
		break;
	}
}

void Fade::Draw() { sprite_->Draw(); }

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;

	if (status == Status::FadeOut) {
		// 小さい黒点からスタート
		scale_ = 0.0f;
		sprite_->SetSize({10.0f, 10.0f});
		sprite_->SetColor({0, 0, 0, 1});
		sprite_->SetPosition({640.0f, 360.0f});
	} else if (status == Status::FadeIn) {
		// 画面全体を覆った状態からスタート
		scale_ = 1.5f;
		sprite_->SetSize({1280.0f * scale_, 720.0f * scale_});
		sprite_->SetColor({0, 0, 0, 1});
		sprite_->SetPosition({640.0f, 360.0f});
	}
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	switch (status_) {
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		return counter_ >= duration_;
	default:
		return true;
	}
}