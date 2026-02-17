#pragma once
#include "PlayerStructs.h"

class PlayerInputController {
public:
	PlayerCommand GetCommand() {
		PlayerCommand cmd;
		auto input = KamataEngine::Input::GetInstance();

		// 移動方向の集計
		if (input->PushKey(DIK_W)) 
			cmd.moveDirection.z += 1.0f;
		if (input->PushKey(DIK_S))
			cmd.moveDirection.z -= 1.0f;
		if (input->PushKey(DIK_D))
			cmd.moveDirection.x += 1.0f;
		if (input->PushKey(DIK_A))
			cmd.moveDirection.x -= 1.0f;

		// トリガー系
		cmd.doAttack = input->TriggerKey(DIK_J);
		cmd.doSpecialAttack = input->TriggerKey(DIK_K);
		cmd.doStep = input->TriggerKey(DIK_H);

		return cmd;
	}
};