#pragma once
#include <cstdint>

enum class PlayerState {
	Idle,
	Move,
	Step,
	Attack,
	Stun,
	Dead,
	Victory
};

class PlayerTextureController {
public:
	void Initialize();

	uint32_t GetTexture(bool isRight, PlayerState state, int walkFrame, bool attackFromRight, float knockDownTimer);

private:
	// --- 右向きテクスチャ ---
	uint32_t RIdle_;
	uint32_t RRun_[3];
	uint32_t RLeftPunch_;
	uint32_t RRightPunch_;
	uint32_t RKnockDown_[2];
	uint32_t RStun_;
	uint32_t RVictory_;

	// --- 左向きテクスチャ ---
	uint32_t LIdle_;
	uint32_t LRun_[3];
	uint32_t LLeftPunch_;
	uint32_t LRightPunch_;
	uint32_t LKnockDown_[2];
	uint32_t LStun_;
};
