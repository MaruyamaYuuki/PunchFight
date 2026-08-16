#pragma once

class TimeManager {
public:
	static TimeManager* GetInstance();

	void Update();

	float GetDeltaTime() const { return deltaTime_; }

	void SetTimeScale(float scale);
	void StartHitStop(float scale, float time);

private:
	float timeScale_ = 1.0f;
	float deltaTime_ = 1.0f / 60.0f;

	float hitStopTimer_ = 0.0f;
};