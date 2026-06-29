#pragma once
#include "PlayerStructs.h"

/// <summary>
/// プレイヤーの入力処理を抽象化・管理するコントローラークラス。
/// キーボードおよびゲームパッド（XInput）の入力状態を毎フレーム監視し、
/// キャラクター制御用の統一コマンド（PlayerCommand構造体）に変換して出力します。
/// デバイス依存の入力処理をこのクラスに集約することで、キャラクター側の移動・アクション処理と入力を分離します。
/// </summary>
class PlayerInputController {
public:

	/// <summary>
	/// 現在のフレームにおけるデバイスの入力状態を取得し、プレイヤーコマンドとして生成します。
	/// キーボードとゲームパッド両方の入力に対応し、両方の入力があった場合は加算・マージされます。
	/// </summary>
	/// <returns>正規化前（加算状態）の移動方向ベクトルと、各アクションのトリガー状態を格納した PlayerCommand</returns>
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