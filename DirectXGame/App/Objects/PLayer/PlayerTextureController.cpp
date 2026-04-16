#include "PlayerTextureController.h"
#include "KamataEngine.h"

using namespace KamataEngine;

void PlayerTextureController::Initialize() {

	    // --- 右 ---
	RIdle_ = TextureManager::Load("playerTextures/RPlayer.png");
	RRun_[0] = TextureManager::Load("playerTextures/RRun1.png");
	RRun_[1] = TextureManager::Load("playerTextures/RRun2.png");
	RRun_[2] = TextureManager::Load("playerTextures/RRun3.png");

	RLeftPunch_ = TextureManager::Load("playerTextures/RLeftPunch.png");
	RRightPunch_ = TextureManager::Load("playerTextures/RRightPunch.png");

	RKnockDown_[0] = TextureManager::Load("playerTextures/RKnockDown.png");
	RKnockDown_[1] = TextureManager::Load("playerTextures/RKnockDown2.png");

	RStun_ = TextureManager::Load("playerTextures/RStun.png");
	RVictory_ = TextureManager::Load("playerTextures/RUppercut.png");

	// --- 左 ---
	LIdle_ = TextureManager::Load("playerTextures/LPlayer.png");
	LRun_[0] = TextureManager::Load("playerTextures/LRun1.png");
	LRun_[1] = TextureManager::Load("playerTextures/LRun2.png");
	LRun_[2] = TextureManager::Load("playerTextures/LRun3.png");

	LLeftPunch_ = TextureManager::Load("playerTextures/LLeftPunch.png");
	LRightPunch_ = TextureManager::Load("playerTextures/LRightPunch.png");

	LKnockDown_[0] = TextureManager::Load("playerTextures/LKnockDown.png");
	LKnockDown_[1] = TextureManager::Load("playerTextures/LKnockDown2.png");

	LStun_ = TextureManager::Load("playerTextures/LStun.png");

}

uint32_t PlayerTextureController::GetTexture(
	bool isRight, 
	PlayerState state, 
	int walkFrame, 
	bool attackFromRight, 
	float knockDownTimer) { 

    // ノックダウン
	if (state == PlayerState::Dead) {
		if (knockDownTimer <= 1.0f) {
			return isRight ? RKnockDown_[1] : LKnockDown_[1];
		}
		return isRight ? RKnockDown_[0] : LKnockDown_[0];
	}

	// スタン
	if (state == PlayerState::Stun) {
		return isRight ? RStun_ : LStun_;
	}

	// 勝利
	if (state == PlayerState::Victory) {
		return RVictory_;
	}

	// 攻撃
	if (state == PlayerState::Attack) {
		if (attackFromRight) {
			return isRight ? RRightPunch_ : LRightPunch_;
		} else {
			return isRight ? RLeftPunch_ : LLeftPunch_;
		}
	}

	// ステップ
	if (state == PlayerState::Step) {
		return isRight ? RRun_[0] : LRun_[0];
	}

	// 移動
	if (state == PlayerState::Move) {
		static const int pattern[4] = {0, 1, 2, 1};
		int index = pattern[walkFrame];
		return isRight ? RRun_[index] : LRun_[index];
	}

	// 待機
	return isRight ? RIdle_ : LIdle_;
}
