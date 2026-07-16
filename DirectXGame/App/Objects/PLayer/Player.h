#pragma once

#include "KamataEngine.h"
#include "PlayerInputController.h"
#include "PlayerTextureController.h"
#include "PlayerCombat.h"
#include "../../../Engine/Math/WorldTransformEx.h"
#include "../../../Engine//Math/Collider.h"
#include "../../../Engine/Particle/SmokeParticleManager.h"

/// <summary>
/// ユーザー操作による自キャラクターの制御と状態管理。
/// 入力デバイス（キーボード /ゲームパッド）からの入力を受け取り、移動や攻撃アクションへ変換する。
/// 攻撃アニメーションのステート（状態）管理。
/// 自身の当たり判定（攻撃用・被弾用）の更新と、カメラに対する位置情報の提供。
/// プレイヤー固有の特殊アクション（回避、必殺技など）のロジック実行。
/// </summary>
namespace MyEngine {
    class GameConfigManager;
}
class Player {
public:
	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~Player() = default;

	/// </summary>
	/// <param name="model">通常時（プレイヤー本体）のモデル</param>
	/// <param name="modelSP">強攻撃演出用の特殊モデル</param>
	/// <param name="modelBox">デバッグ用ヒットボックス表示モデル</param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelSP, KamataEngine::Model* modelBox);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

	/// <summary>
	/// 強攻撃の気弾単体の描画関数
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawSpecial(KamataEngine::Camera& camera);

	/// <summary>
	/// プレイヤーの状態のリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// WorldTransformだけを更新
	/// </summary>
	void UpdateWorldTransform();

	/// <summary>
	/// 接触判定
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	void OnHit(int32_t damage);

	/// <summary>
	/// プレイヤーのWorldTransformを設定する
	/// </summary>
	/// <returns>プレイヤーのWorldTransform</returns>
	const MyEngine::WorldTransformEx& GetWorldTransform() const { return worldTransform_; }

	/// <summary>
	/// プレイヤーの移動量を取得する
	/// </summary>
	/// <returns>プレイヤーの移動量</returns>
	const KamataEngine::Vector3& GetVelocity() const { return move_; }

	/// <summary>
	/// プレイヤーの死亡判定
	/// </summary>
	/// <returns>死亡していれば true、生存していれば false</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// プレイヤーのHPを取得する
	/// </summary>
	/// <returns>現在の体力値（0以下で死亡状態へ遷移）</returns>
	int32_t GetHP() const { return HP_; }

	/// <summary>
	/// プレイヤーのヒットボックスを取得する
	/// </summary>
	/// <returns>当たり判定の情報（位置・サイズ・有効状態など）</returns>
	HitBox GetPlayerHitBox() const { return playerHitBox_; }

	/// <summary>
	/// 攻撃のヒットボックスを取得する
	/// </summary>
	/// <returns>攻撃判定の情報（位置・サイズ・有効状態など）</returns>
	const HitBox& GetAttackHitBox() const { return combat_->GetNormalHitBox(); }

	/// <summary>
	/// 強攻撃のヒットボックスを取得する
	/// </summary>
	/// <returns>強攻撃判定の情報（位置・サイズ・有効状態など）</returns>
	const HitBox& GetSPAttackHitBox() const { return combat_->GetSpecialHitBox(); }

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>プレイヤーの攻撃力</returns>
	int32_t GetAttackPower() const { return combat_->GetNormalPower(); }

	/// <summary>
	/// 強攻撃の攻撃力を取得する
	/// </summary>
	/// <returns>強攻撃の攻撃力</returns>
	int32_t GetSPAttackPower() const { return combat_->GetSpecialPower(); }

	/// <summary>
	/// プレイヤーの向いてる方向を取得する
	/// </summary>
	/// <returns>右向きなら 1.0f、左向きなら -1.0f を返す</returns>
	float GetFacingDir() const { return facingDir_; }

	/// <summary>
	/// 強攻撃の発射方向を取得
	/// </summary>
	/// <returns>右方向なら 1.0f、左方向なら -1.0f を返す</returns>
	float GetSPAttackDir() const { return combat_->GetSpecialDir(); }

	/// <summary>
	/// 目標座標に着いたか
	/// </summary>
	/// <returns>目標座標に着いているなら true、着いてないなら false</returns>
	bool IsGoal() { return isGoal_; }

	/// <summary>
	/// ポーズをとったか
	/// </summary>
	/// <returns>ポーズをとっているなら true、とってないなら false</returns>
	bool IsVictory() { return isVictory_; }

	/// <summary>
	/// ステップ中かを取得
	/// </summary>
	/// <returns>ステップ中なら true、してなければ false</returns>
	bool CanStep() const { return !isStepping_ && stepTimer_ <= 0; }

	/// <summary>
	/// ステップのクールタイムを取得
	/// </summary>
	/// <returns>ステップのクールタイム</returns>
	int32_t GetStepCooldownTimer() const { return stepTimer_; }

	/// <summary>
	/// ステップのクールタイムの最大値を取得
	/// </summary>
	/// <returns>ステップのクールタイムの最大値</returns>
	int32_t GetStepCooldownMax() const { return stepCooldown_; }

	bool IsSpecial() const { return combat_->IsSpecialAttacking(); }

	/// <summary>
	/// 強攻撃を使ったかを取得
	/// </summary>
	/// <returns>使っていれば true、いなければ false</returns>
	bool CanSpecialAttack() const { return combat_->CanSpecial(); }

	/// <summary>
	/// 強攻撃のクールタイムを取得
	/// </summary>
	/// <returns>強攻撃のクールタイム</returns>
	float GetSPAttackCooldownTimer() const { return combat_->GetSpecialCooldownTimer(); }

	/// <summary>
	/// 強攻撃のクールタイムの最大値を取得
	/// </summary>
	/// <returns>強攻撃のクールタイムの最大値</returns>
	float GetSPAttackCooldownMax() const { return combat_->GetSpecialCooldown(); }

	/// <summary>
	/// 強攻撃（スペシャル）が実行された瞬間の判定
	/// </summary>
	/// <returns>このフレームで強攻撃を発動した直後なら true</returns>
	bool DidSpecialAttack() const { return combat_->DidStartSpecialAttack(); }

	KamataEngine::Vector3 GetSpecialPos() const { return worldTransformSP_.translation_; } 

	/// <summary>
	/// プレイヤーの体力を設定する
	/// </summary>
	/// <param name="hp">体力</param>
	void SetHP(int32_t hp) { HP_ = hp; }

	/// <summary>
	/// X軸での移動限界座標を設定する
	/// </summary>
	/// <param name="limit">移動限界</param>
	void SetEndMoveLimitX(float limit) { endMoveLimitX_ = limit; }

	/// <summary>
	/// プレイヤーの座標を設定する
	/// </summary>
	/// <param name="pos">座標</param>
	void SetTranslation(const KamataEngine::Vector3& pos) { worldTransform_.translation_ = pos; }

	/// <summary>
	/// X軸での回転角を設定する
	/// </summary>
	/// <param name="rotX">回転角</param>
	void SetRotateX(float rotX) { worldTransform_.rotation_.x = rotX; }

	/// <summary>
	/// プレイヤーの移動制限をするかどうかを設定する
	/// </summary>
	/// <param name="flag">移動制限許可フラグ</param>
	void SetMoveLimitEnabled(bool flag) { enableMoveLimit_ = flag; }

	/// <summary>
	/// クリアシーン用の演出アニメーション制御
	/// </summary>
	void ClearAnimation();

private:
	/// <summary>
	/// 入力に基づいた移動計算（通常移動、ステップ）の実行
	/// </summary>
	void Move(const PlayerCommand& cmd);

    /// <summary>
	/// ステップ（回避・ダッシュ）実行中の座標移動を適用する。
	/// ステップ用ベクトルの減衰処理や、経過時間による移動量の制御を行う。
	/// </summary>
	void ApplyStepMovement();

	/// <summary>
	/// スティックやキー入力に基づいた通常移動の座標加算を行う。
	/// 斜め移動時の速度正規化や、基本移動スピードの適用が含まれる。
	/// </summary>
	void ApplyNormalMovement();

	/// <summary>
	/// プレイヤーの座標を画面外や進行制限エリア内に収まるよう補正する。
	/// 壁際での座標クランプ処理。
	/// </summary>
	void ConstrainPosition();

	/// <summary>
	/// 移動に伴う視覚演出（足元の土煙パーティクル等）の発生と更新を行う。
	/// </summary>
	void UpdateMoveEffects();

	/// <summary>
	/// アニメーション用のフレームカウンタを更新する。
	/// 経過時間に応じてテクスチャの切り替えインデックスを進める。
	/// </summary>
	void UpdateAnimationFrames();

	/// <summary>
	/// 現在の状態（移動・攻撃・ダメージ）に応じた描画テクスチャの切り替え
	/// </summary>
	void TextureUpdate();

	/// <summary>
	/// 外部から受け取った PlayerCommand オブジェクトを解析し、
	/// プレイヤーの各ステート（移動、攻撃、回避）へ命令を分配する。
	/// </summary>
	/// <param name="cmd">解析対象の入力コマンド構造体</param>
	void ApplyCommand(const PlayerCommand& cmd);

private:
	std::unique_ptr<PlayerInputController> inputController_;
	std::unique_ptr<PlayerTextureController> textureController_;
	std::unique_ptr<PlayerCombat> combat_;

	MyEngine::WorldTransformEx worldTransform_;
	MyEngine::WorldTransformEx worldTransformSP_;
	MyEngine::WorldTransformEx worldTransformNormalAttackHitBox_;
	MyEngine::WorldTransformEx worldTransformSPHitBox_;
	MyEngine::WorldTransformEx worldTransformPHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelSpecial_ = nullptr;
	KamataEngine::Model* modelDebugHitBox_ = nullptr;
	KamataEngine::Model* modelHitBox_ = nullptr;
	KamataEngine::Model* modelSPHitBox_ = nullptr;

	// --- プレイヤーのテクスチャ ---
	uint32_t textureHandle_ = 0;

	// --- 強攻撃のテクスチャ ---
	uint32_t SPTextureHandle_ = 0;
	uint32_t RSpecialTexture_ = 0;
	uint32_t LSpecialTexture_ = 0;

	// --- 足元のパーティクルのテクスチャ ---
	uint32_t smokeTexture_ = 0;

	// --- SE ---
	uint32_t normalAttackSEDataHandle_ = 0;
	uint32_t spAttackSEDataHandle_ = 0;
	uint32_t hitSEDataHandle_ = 0;

	uint32_t normalAttackSEVoiceHandle_ = 0;
	uint32_t spAttackSEVoiceHandle_ = 0;
	uint32_t hitSEVoiceHandle_ = 0;

	bool isDead_ = false;
	int32_t HP_;
	int32_t maxHP_;
	KamataEngine::Vector3 kInitialPos_;

	// --- 移動関連 ---
	KamataEngine::Vector3 move_ = {0, 0, 0};
	float moveSpeed_; // 通常移動速度
	int32_t walkFrame_ = 0;         // 0〜3でループ
	int32_t walkFrameTimer_ = 0;    // テクスチャ切替タイマー
	int32_t walkFrameInterval_; // 何フレームごとに切り替えるか
	// 移動限界座標
	float startMoveLimitX = 3.0f;
	float moveLimitZ = 3.5f;
	float minMoveLimitZ = 2.5f;

	// --- ステップ関連 ---
    bool isStepping_ = false;
	KamataEngine::Vector3 stepDirection_ = {0, 0, 0};
	int32_t stepTimer_ = 0;
	int32_t stepCooldown_; // 例：1秒クールタイム（60FPS想定）
	int32_t stepFrame_ = 0;
	float stepPower_ ; // 通常移動の3倍速

    // --- 攻撃管理 ---
	float facingDir_ = 1.0f;      // 向き（1.0f：右, -1.0f：左）
	const float deltaTime_ = 1.0f / 60.0f;


	// --- ヒットボックス ---
	HitBox playerHitBox_;

	// --- ノックダウン処理用 ---
	float knockDownTimer_;
	float knockDownDuration_;

	float endMoveLimitX_ = 0;

	// --- スタン処理用 ---
	int stunTimer_;

	// ---クリア演出用 ---
	bool isGoal_ = false;
	bool isVictory_ = false;
	bool enableMoveLimit_ = true;
	float poseWaitTimer_;

	// --- パーティクル用 ---
	std::unique_ptr<MyEngine::SmokeParticleManager> smokeManager_;
	float trailSpawnTimer_ = 0.0f;
	float trailSpawnInterval_;
	KamataEngine::Vector3 smokeSize_;

};
