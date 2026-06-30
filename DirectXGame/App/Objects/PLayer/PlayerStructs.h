#pragma once
#include "KamataEngine.h"

/// <summary>
/// プレイヤーの1フレーム分の入力コマンド（操作意図）をまとめたデータ構造体。
/// PlayerInputControllerによって生成され、Playerクラスの更新処理に渡されます。
/// 入力デバイス（キーボードやパッド）の物理的な状態ではなく、「ゲーム内で何をしたいか」という
/// 抽象化された論理コマンドとして扱われます。
/// </summary>
struct PlayerCommand {
	KamataEngine::Vector3 moveDirection = {0, 0, 0}; // 移動したい方向
	bool doAttack = false;                           // 攻撃ボタンが押されたか
	bool doSpecialAttack = false;                    // 特殊攻撃ボタンが押されたか
	bool doStep = false;                             // ステップボタンが押されたか
};