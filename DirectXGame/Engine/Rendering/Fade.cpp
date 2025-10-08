#include "Fade.h"
#define NOMINMAX
#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

Fade::~Fade() { delete sprite_; }

void Fade::Initialize() {
	textureHandle_ = TextureManager::Load("black.png");
	sprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	sprite_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void Fade::Update() {
	if (status_ == Status::None) {
		return;
	}

	// フェード状態による分岐
	switch (status_) {
	case Fade::Status::None:
		// 何もしない
		break;
	case Fade::Status::FadeIn:
		// 1フレーム分の秒数をカウントダウン
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor({0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)});
		break;
	case Fade::Status::FadeOut:
		// 1フレーム分の秒数をカウントダウン
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor({0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)});
		break;
	}
}

void Fade::Draw() { sprite_->Draw(); }

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	// フェード状態による分岐
	switch (status_) {
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
		break;
	}

	return true;
}
