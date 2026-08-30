#pragma once
#include <cstdint>

/// <summary>
/// プレイヤーの論理的な行動状態を定義する列挙型
/// </summary>
enum class PlayerState {
	Idle,
	Move,
	Step,
	Attack,
	Uppercut,
	Stun,
	Dead,
	Victory
};

/// <summary>
/// プレイヤーのテクスチャリソース管理と、状態に応じたハンドルの切り替えを担当するクラス。
/// 左右の向き、アニメーションフレーム、攻撃の左右判定、タイマーによる表情変化などを
/// 一括して引き受け、描画に必要なテクスチャハンドルを決定します。
/// </summary>
class PlayerTextureController {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 現在のプレイヤーの状況を入力し、描画すべき最適なテクスチャハンドルを取得する。
	/// </summary>
	/// <param name="isRight">プレイヤーが右を向いているか</param>
	/// <param name="state">現在の行動ステート</param>
	/// <param name="walkFrame">移動アニメーションの現在のフレーム番号</param>
	/// <param name="attackFromRight">（攻撃中のみ）右パンチか左パンチか</param>
	/// <param name="knockDownTimer">（死亡中のみ）経過時間によるダウン状態の遷移用タイマー</param>
	/// <returns>描画に使用するテクスチャハンドル</returns>
	uint32_t GetTexture(bool isRight, PlayerState state, int walkFrame, bool attackFromRight, float knockDownTimer);

private:
	// --- 右向きテクスチャ ---
	uint32_t RIdle_;
	uint32_t RRun_[3];
	uint32_t RLeftPunch_;
	uint32_t RRightPunch_;
	uint32_t RUppercut_;
	uint32_t RKnockDown_[2];
	uint32_t RStun_;
	uint32_t RVictory_;

	// --- 左向きテクスチャ ---
	uint32_t LIdle_;
	uint32_t LRun_[3];
	uint32_t LLeftPunch_;
	uint32_t LUppercut_;
	uint32_t LRightPunch_;
	uint32_t LKnockDown_[2];
	uint32_t LStun_;
};
