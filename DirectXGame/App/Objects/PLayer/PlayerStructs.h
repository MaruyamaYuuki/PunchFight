#pragma once
#include "KamataEngine.h"

struct PlayerCommand {
	KamataEngine::Vector3 moveDirection = {0, 0, 0}; // 移動したい方向
	bool doAttack = false;                           // 攻撃ボタンが押されたか
	bool doSpecialAttack = false;                    // 特殊攻撃ボタンが押されたか
	bool doStep = false;                             // ステップボタンが押されたか
};