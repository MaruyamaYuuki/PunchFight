#pragma once
#include "../../../Engine/Math/Collider.h"
#include "KamataEngine.h"

/// <summary>
/// プレイヤーの攻撃処理（通常・強攻撃）に特化したロジック管理クラス。
/// 攻撃ステートの遷移、ヒットボックスの座標更新、クールタイム管理、
/// および遠距離攻撃（気弾等）の移動計算を担当します。
/// </summary>
namespace MyEngine {
    class GameConfigManager;
}
class PlayerCombat {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="playerPos">プレイヤーの現在座標</param>
	/// <param name="facingDir">プレイヤーの現在の向き（1.0f または -1.0f）</param>
	void Update(const KamataEngine::Vector3& playerPos, float facingDir);

/// <summary>
	/// 通常攻撃（コンボパンチ）を開始する。
	/// クールタイム中、または既に攻撃中の場合は実行されない。
	/// </summary>
	/// <param name="playerPos">攻撃開始時のプレイヤー座標</param>
	/// <param name="facingDir">攻撃を繰り出す方向</param>
	void StartNormalAttack(KamataEngine::Vector3 playerPos, float facingDir);

	/// <summary>
	/// 強攻撃（遠距離気弾、または特殊突進）を開始する。
	/// 攻撃オブジェクトの初期位置と移動方向を確定させ、クールタイムを開始する。
	/// </summary>
	/// <param name="playerPos">発射時のプレイヤー座標</param>
	/// <param name="facingDir">発射方向</param>
	void StartSpecialAttack(KamataEngine::Vector3 playerPos, float facingDir);

	// --- Getter ---

	/// <summary> 通常攻撃の当たり判定情報を取得 </summary>
	const HitBox& GetNormalHitBox() const { return normalHitBox_; }

	/// <summary> 強攻撃の当たり判定情報を取得 </summary>
	const HitBox& GetSpecialHitBox() const { return specialHitBox_; }

	/// <summary> 通常攻撃のダメージ量を取得 </summary>
	int GetNormalPower() const { return normalPower_; }

	/// <summary> 強攻撃のダメージ量を取得 </summary>
	int GetSpecialPower() const { return specialPower_; }

	/// <summary> 現在通常攻撃の動作中（持続時間内）であるかを取得 </summary>
	bool IsNormalAttacking() const { return isNormalAttacking_; }

	/// <summary> 現在強攻撃の動作中（気弾が画面内に存在等）であるかを取得 </summary>
	bool IsSpecialAttacking() const { return isSpecialAttacking_; }

	/// <summary> このフレームに通常攻撃が開始されたかのトリガーフラグを取得 </summary>
	bool DidStartNormalAttack() const { return justStartedNormal_; }

	/// <summary> このフレームに強攻撃が開始されたかのトリガーフラグを取得 </summary>
	bool DidStartSpecialAttack() const { return justStartedSpecial_; }

	/// <summary> 強攻撃が放たれた際の進行方向を取得 </summary>
	float GetSpecialDir() const { return specialDir_; }

	/// <summary> 強攻撃オブジェクト（気弾等）の現在座標を取得 </summary>
	KamataEngine::Vector3 GetSpecialPos() const { return specialPos_; }

	/// <summary> 強攻撃が使用可能（クールタイムが終了している）かを取得 </summary>
	bool CanSpecial() const { return canSpecialAttack_; }

	/// <summary> 左右どちらのパンチを繰り出すべきか（演出用フラグ）を取得 </summary>
	bool GetAttackForRight() const { return attackFromRight_; }

	/// <summary> 強攻撃のクールタイム残り時間を取得 </summary>
	float GetSpecialCooldownTimer() const { return specialCooldownTimer_; }

	/// <summary> 設定された強攻撃の最大クールタイム値を取得 </summary>
	float GetSpecialCooldown() const { return specialCooldown_; }

private:

	// ===== 通常攻撃 =====
	bool isNormalAttacking_ = false;
	bool canNormalAttack_ = true;
	bool justStartedNormal_ = false;
	int normalTimer_ = 0;
	int normalCooldownTimer_ = 0;

	int normalPower_;
	int normalDuration_;
	int normalCooldown_;

	bool attackFromRight_ = true;

	HitBox normalHitBox_;

	// ===== 強攻撃 =====
	bool isSpecialAttacking_ = false;
	bool canSpecialAttack_ = true;
	bool justStartedSpecial_ = false;

	float specialTimer_ = 0.0f;
	float specialCooldownTimer_ = 0.0f;

	float specialDuration_;
	float specialCooldown_;
	float specialSpeed_;
	int specialPower_;

	float specialDir_ = 1.0f;

	KamataEngine::Vector3 specialPos_;
	HitBox specialHitBox_;

	const float deltaTime_ = 1.0f / 60.0f;
};