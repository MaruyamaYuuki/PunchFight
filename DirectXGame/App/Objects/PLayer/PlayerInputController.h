#pragma once
#include "PlayerStructs.h"

class PlayerInputController {
public:
	PlayerCommand GetCommand() {

		PlayerCommand cmd;
		auto input = KamataEngine::Input::GetInstance();

		// ----------------
		// キーボード移動
		// ----------------
		if (input->PushKey(DIK_W))
			cmd.moveDirection.z += 1.0f;
		if (input->PushKey(DIK_S))
			cmd.moveDirection.z -= 1.0f;
		if (input->PushKey(DIK_D))
			cmd.moveDirection.x += 1.0f;
		if (input->PushKey(DIK_A))
			cmd.moveDirection.x -= 1.0f;

		// ----------------
		// キーボード攻撃
		// ----------------
		cmd.doAttack = input->TriggerKey(DIK_J);
		cmd.doSpecialAttack = input->TriggerKey(DIK_K);
		cmd.doStep = input->TriggerKey(DIK_H);

		// ----------------
		// ゲームパッド
		// ----------------
		XINPUT_STATE state{};
		XINPUT_STATE preState{};

		bool hasPad = input->GetJoystickState(0, state);
		bool hasPadPrev = input->GetJoystickStatePrevious(0, preState);

		if (hasPad) {

			// 左スティック
			float lx = state.Gamepad.sThumbLX / 32767.0f;
			float ly = state.Gamepad.sThumbLY / 32767.0f;

			const float deadZone = 0.2f;

			if (fabs(lx) > deadZone)
				cmd.moveDirection.x += lx;
			if (fabs(ly) > deadZone)
				cmd.moveDirection.z += ly;
		}

		if (hasPad && hasPadPrev) {

			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {
				cmd.doAttack = true;
			}

			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)) {
				cmd.doSpecialAttack = true;
			}

			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
				cmd.doStep = true;
			}
		}

		return cmd;
	}
};