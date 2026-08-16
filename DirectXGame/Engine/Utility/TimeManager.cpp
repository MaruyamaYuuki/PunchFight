#include "TimeManager.h"

TimeManager* TimeManager::GetInstance() {
	static TimeManager instance;
	return &instance;
}

void TimeManager::Update() {
	// ヒットストップ中
	if (hitStopTimer_ > 0.0f) {
		hitStopTimer_ -= 1.0f / 60.0f;

		if (hitStopTimer_ <= 0.0f) {
			timeScale_ = 1.0f;
		}
	}

	deltaTime_ = (1.0f / 60.0f) * timeScale_;
}

void TimeManager::SetTimeScale(float scale) { timeScale_ = scale; }

void TimeManager::StartHitStop(float scale, float time) {
	timeScale_ = scale;
	hitStopTimer_ = time;
}