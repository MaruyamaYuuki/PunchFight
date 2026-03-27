#pragma once
#include "UICommand.h"
#include <KamataEngine.h>
#include <cmath>

namespace MyEngine {
class UIInputController {
public:
	UICommand GetCommand() {
		UICommand cmd{};

		auto input = KamataEngine::Input::GetInstance();

		//================
		// キーボード
		//================

		if (input->TriggerKey(DIK_W)) {
			cmd.move = -1;
		}

		if (input->TriggerKey(DIK_S)) {
			cmd.move = 1;
		}

		if (input->TriggerKey(DIK_A)) {
			cmd.moveLR = -1;
		}

		if (input->TriggerKey(DIK_D)) {
			cmd.moveLR = 1;
		}

		if (input->TriggerKey(DIK_RETURN)) {
			cmd.decide = true;
		}

		if (input->TriggerKey(DIK_ESCAPE)) {
			cmd.pause = true;
		}

		//================
		// ゲームパッド
		//================

		XINPUT_STATE state{};
		XINPUT_STATE preState{};

		bool hasPad = input->GetJoystickState(0, state);
		bool hasPadPrev = input->GetJoystickStatePrevious(0, preState);

		if (hasPad && hasPadPrev) {

			// START（ポーズ）
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_START)) {

				cmd.pause = true;
			}

			// Bボタン（決定）
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_B)) {

				cmd.decide = true;
			}

			// 十字キー
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) {

				cmd.move = -1;
			}

			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) {

				cmd.move = 1;
			}

			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) {

				cmd.moveLR = -1;
			}

			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && !(preState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) {

				cmd.moveLR = 1;
			}
		}

		//================
		// 左スティック
		//================

		if (hasPad) {

			float ly = state.Gamepad.sThumbLY / 32767.0f;
			float ply = preState.Gamepad.sThumbLY / 32767.0f;

			const float deadZone = 0.5f;

			if (ly > deadZone && ply <= deadZone) {
				cmd.move = -1;
			}

			if (ly < -deadZone && ply >= -deadZone) {
				cmd.move = 1;
			}

			float lx = state.Gamepad.sThumbLX / 32767.0f;
			float plx = preState.Gamepad.sThumbLX / 32767.0f;

			if (lx > deadZone && plx <= deadZone) {
				cmd.moveLR = 1;
			}

			if (lx < -deadZone && plx >= -deadZone) {
				cmd.moveLR = -1;
			}
		}

		return cmd;
	}
};
}

