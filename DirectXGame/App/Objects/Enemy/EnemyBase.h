#pragma once
#include <memory>
#include "../../../Engine/Math/Collider.h"
#include "../../../Engine/Math/WorldTransformEx.h"
#include "../../../Engine/Particle/SmokeParticle.h"
#include "../../../Engine/Particle/SmokeParticleManager.h"
#include "../../../Engine//Particle/DustParticleManager.h"
#include "KamataEngine.h"
#include "EnemyState.h"

/// <summary>
/// 敵の情報
/// </summary>
struct EnemyData {
	std::string modelPath;
	float speed;
	int32_t hp;
	int32_t attackPower;
};

/// <summary>
/// 敵の状態
/// </summary>
enum class EnemyState { 
	Idle = 0, 
	Walking, 
	AttackWait,
	Attacking, 
	Stunned, 
	Knockback, 
	Dead 
};

/// <summary>
/// 全エネミーの共通インターフェースおよび共通ロジックの提供。
/// HP、座標、速度などの共通ステータスの保持。
/// 被弾時のダメージ計算、ノックバック物理、スタン状態の処理。
/// 派生クラスが実装すべき「攻撃AI」等の純粋仮想関数の定義。
/// 共通の描画処理（Zソート対応の描画パス）の実行。
/// </summary>
namespace MyEngine {
    class GameConfigManager;
}
class EnemyBase {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	EnemyBase() = default;

	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	virtual ~EnemyBase() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data">敵の情報</param>
	virtual void Initialize(const EnemyData& data);

	/// 更新処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標（追従対象）</param>
	/// <param name="allEnemies">全エネミーのリスト（他の敵との重なり回避計算に使用）</param>
	virtual void Update(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(KamataEngine::Camera& camera);

	///
	/// トランスフォーム・移動関連
	/// 
	
	// --- Getter ---

	/// <summary>
	/// 敵の現在座標を取得する
	/// </summary>
	/// <returns>ワールド座標（Vector3）</returns>
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	/// <summary>
	/// 敵の移動スピードを取得する
	/// </summary>
	/// <returns>移動速度（float）</returns>
	float GetSpeed() const { return speed_; }

	/// <summary>
	/// 敵の向いている方向を取得する
	/// </summary>
	/// <returns>左なら -1.0f、 右なら 1.0f</returns>
	float GetFacingDir() const { return facingDir_; }

	/// <summary>
	/// 敵の攻撃方向を取得する
	/// </summary>
	/// <returns>左なら -1.0f、 右なら 1.0f</returns>
	float GetAttackDirX() const { return attackDirX_; }

	// --- Setter ---

	/// <summary>
	/// 敵の座標を直接設定する
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	void SetPosition(float x, float y, float z);

	/// <summary>
	/// 敵の大きさを設定する
	/// </summary>
	/// <param name="scale">スケーリング値</param>
	void SetScale(const KamataEngine::Vector3& scale);

	/// <summary>
	/// 敵の向き（左右）を設定する
	/// </summary>
	/// <param name="dir">方向値（1.0f または -1.0f）</param>
	void SetFacingDir(float dir) { facingDir_ = dir; }

	/// <summary>
	/// 敵の攻撃の向き（左右）を設定する
	/// </summary>
	/// <param name="dir">方向値（1.0f または -1.0f）</param>
	void SetAttackDirX(float dir) { attackDirX_ = dir; }

	/// <summary>
	/// X座標に値を加算する（移動用ヘルパー）
	/// </summary>
	/// <param name="x">加算するX量（正の数で右、負の数で左）</param>
	void AddPositionX(float x) { worldTransform_.translation_.x += x; }

	///
	/// ステータス・状態管理関連
	/// 
	
	// --- Getter ---

	/// <summary>
	/// 現在のHPを取得する
	/// </summary>
	/// <returns>残りHP</returns>
	int32_t GetHP() const { return hp_; }

	/// <summary>
	/// 敵の死亡判定を取得する
	/// </summary>
	/// <returns>死亡していれば true</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// ノックバック中かどうかを取得する
	/// </summary>
	/// <returns>ノックバック中なら true</returns>
	bool IsKnockBack() const { return isKnockBack_; }

	/// <summary>
	/// 現在の行動状態（ステート）を取得する
	/// </summary>
	/// <returns>EnemyState列挙型</returns>
	EnemyState GetState() const { return state_; }

	// --- Setter ---

	/// <summary>
	/// 行動状態（ステート）を直接設定する
	/// </summary>
	/// <param name="state">設定するEnemyState</param>
	void SetState(EnemyState state) { state_ = state; }

	///
	/// 攻撃・判定(HitBox)関連
	/// 
	
	// --- Getter ---

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>攻撃力数値</returns>
	int32_t GetAttackPower() const { return attackPower_; }

	/// <summary>
	/// 攻撃射程距離を取得する
	/// </summary>
	/// <returns>射程距離（float）</returns>
	float GetAttackRange() const { return attackRange_; }

	/// <summary>
	/// 敵自身の被弾判定（ヒットボックス）を取得する
	/// </summary>
	/// <returns>HitBox構造体への参照</returns>
	const HitBox& GetHitBox() const { return hitBox_; }

	/// <summary>
	/// 敵の攻撃判定（ヒットボックス）を取得する
	/// </summary>
	/// <returns>HitBox構造体への参照</returns>
	const HitBox& GetAttackHitBox() const { return attackHitBox_; }

	/// <summary>
	/// 攻撃判定が現在アクティブ（有効）かを取得する
	/// </summary>
	/// <returns>有効なら true</returns>
	bool IsAttackHitBoxActive() const { return attackHitBox_.active; }

	/// <summary>
	/// 攻撃アクション中（予備動作含む）であるかを取得する
	/// </summary>
	/// <returns>攻撃中なら true</returns>
	bool IsAttacking() const { return isAttacking_; }

	/// <summary>
	/// 攻撃モード（索敵範囲に入り攻撃体制）であるかを取得する
	/// </summary>
	/// <returns>モード中なら true</returns>
	bool IsAttackMode() const { return isAttackMode_; }

	/// <summary>
	/// 攻撃後のクールタイム残り時間を取得する
	/// </summary>
	/// <returns>残り秒数</returns>
	float GetAttackCoolDownTimer() const { return attackCooldownTimer_; }

	/// <summary>
	/// この攻撃ですでにダメージを与えたかを取得する（多段ヒット防止用）
	/// </summary>
	/// <returns>ダメージ済みなら true</returns>
	bool HasDealtDamage() const { return hasDealtDamage_; }

	// --- Setter ---

	/// <summary>
	/// 敵自身の被弾判定（ヒットボックス）のパラメータを設定する
	/// </summary>
	/// <param name="center">中心オフセット座標</param>
	/// <param name="size">判定サイズ</param>
	void SetHitBox(const KamataEngine::Vector3& center, const KamataEngine::Vector3& size);

	/// <summary>
	/// 攻撃判定（ヒットボックス）のパラメータを一括設定する
	/// </summary>
	/// <param name="pos">ワールド座標</param>
	void SetAttackHitBox(const KamataEngine::Vector3& pos);

	/// <summary>
	/// 攻撃モード（フラグ）を切り替える
	/// </summary>
	/// <param name="flag">有効なら true</param>
	void SetAttackMode(bool flag) { isAttackMode_ = flag; }

	/// <summary>
	/// 攻撃アクション実行中フラグを切り替える
	/// </summary>
	/// <param name="flag">実行中なら true</param>
	void SetAttacking(bool flag) { isAttacking_ = flag; }

	/// <summary>
	/// 攻撃当たり判定の有効・無効を個別に切り替える
	/// </summary>
	/// <param name="flag">有効にするなら true</param>
	void SetAttackHitBoxActive(bool flag) { attackHitBox_.active = flag; }

	/// <summary>
	/// 攻撃当たり判定の位置のみを更新する
	/// </summary>
	/// <param name="pos">ワールド座標</param>
	void SetAttackHitBoxPos(const KamataEngine::Vector3& pos) { attackHitBox_.pos = pos; }

	/// <summary>
	/// 攻撃当たり判定のサイズのみを更新する
	/// </summary>
	/// <param name="size">サイズ（Vector3）</param>
	void SetAttackHitBoxSize(const KamataEngine::Vector3& size) { attackHitBox_.size = size; }

	/// <summary>
	/// ダメージ済みフラグを設定する
	/// </summary>
	/// <param name="flag">設定する真偽値</param>
	void SetHasDealtDamage(bool flag) { hasDealtDamage_ = flag; }

	// --- Timer Update ---

	/// <summary>
	/// 攻撃クールタイムを更新する（0.0fでクランプ処理付き）
	/// </summary>
	/// <param name="dt">経過時間（デルタタイム）</param>
	void UpdateAttackCooldown(float dt) {
		attackCooldownTimer_ -= dt;
		if (attackCooldownTimer_ < 0.0f) {
			attackCooldownTimer_ = 0.0f;
		}
	}

	/// <summary>
	/// 攻撃クールタイムを最大値にリセットする
	/// </summary>
	void ResetAttackCooldown() { attackCooldownTimer_ = attackCooldown_; }

	///
	/// システム・リソース関連
	///

	// --- Getter ---

	/// <summary>
	/// 敵のワールドトランスフォームを取得する
	/// </summary>
	/// <returns>worldTransform</returns>
	MyEngine::WorldTransformEx& GetWorldTransform() { return worldTransform_; }

	/// <summary>
	/// フレーム間の固定デルタタイムを取得する
	/// </summary>
	/// <returns>経過時間（秒）</returns>
	float GetDeltaTime() const { return deltaTime_; }

	/// <summary>
	/// オーディオマネージャーへのポインタを取得する
	/// </summary>
	/// <returns>Audioポインタ</returns>
	KamataEngine::Audio* GetAudio() const { return audio_; }

	/// <summary>
	/// テクスチャハンドルを取得する
	/// </summary>
	/// <returns>テクスチャハンドル</returns>
	uint32_t GetTextureHandle() const { return textureHandle_; }

	/// <summary>
	/// 右向きの停止中テクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きの停止中テクスチャハンドル</returns>
	virtual uint32_t GetRIdleTexture() const { return RIdleTexture_; }

	/// <summary>
	/// 右向きの攻撃待機テクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きの攻撃待機テクスチャハンドル</returns>
	virtual uint32_t GetRWaitTexture() const { return RWaitTexture_; }

	/// <summary>
	/// 右向きの攻撃テクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きの攻撃テクスチャハンドル</returns>
	virtual uint32_t GetRAttackTexture() const { return RAttackTexture_; }

	/// <summary>
	/// 右向きのスタンテクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きのスタンテクスチャハンドル</returns>
	virtual uint32_t GetRStunTexture() const { return RStunTexture_; }

	/// <summary>
	/// 左左向きの停止中テクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きの停止中テクスチャハンドル</returns>
	virtual uint32_t GetLIdleTexture() const { return LIdleTexture_; }

	/// <summary>
	/// 左向きの攻撃待機テクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きの攻撃待機テクスチャハンドル</returns>
	virtual uint32_t GetLWaitTexture() const { return LWaitTexture_; }

	/// <summary>
	/// 左向きの攻撃テクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きの攻撃テクスチャハンドル</returns>
	virtual uint32_t GetLAttackTexture() const { return LAttackTexture_; }

	/// <summary>
	/// 右向きのスタンテクスチャハンドルを取得する。
	/// </summary>
	/// <returns>右向きのスタンテクスチャハンドル</returns>
	virtual uint32_t GetLStunTexture() const { return LStunTexture_; }

	/// <summary>
	/// 右向き歩行アニメーションの特定のテクスチャを取得する
	/// </summary>
	/// <param name="index">テクスチャの番号</param>
	/// <returns>右向き歩行アニメーションの特定のテクスチャ</returns>
	virtual uint32_t GetRWalkTexture(int index) const { return RWalkTexture_[index]; }

	/// <summary>
	/// 左向き歩行アニメーションの特定のテクスチャを取得する
	/// </summary>
	/// <param name="index">テクスチャの番号</param>
	/// <returns>左向き歩行アニメーションの特定のテクスチャ</returns>
	virtual uint32_t GetLWalkTexture(int index) const { return LWalkTexture_[index]; }

	/// <summary>
	/// 現在の歩行アニメーションフレーム番号を取得する
	/// </summary>
	/// <returns>0〜3のフレーム番号</returns>
	virtual int32_t GetWalkFrame() const { return walkFrame_; }

	/// --- Setter ---

	/// <summary>
	/// テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetTextureHandle(uint32_t handle) { textureHandle_ = handle; }

	/// <summary>
	/// 右向きの停止中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetRIdleTexture(uint32_t handle) { RIdleTexture_ = handle; }

	/// <summary>
	/// 右向きの攻撃待機中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetRWaitTexture(uint32_t handle) { RWaitTexture_ = handle; }

	/// <summary>
	/// 右向きの攻撃中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetRAttackTexture(uint32_t handle) { RAttackTexture_ = handle; }

	/// <summary>
	/// 右向きのスタン中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetRStunTexture(uint32_t handle) { RStunTexture_ = handle; }

	/// <summary>
	/// 右向き歩行アニメーションの特定のテクスチャを設定する
	/// </summary>
	/// <param name="index">テクスチャ番号</param>
	/// <param name="handle">テクスチャハンドル</param>
	void SetRWalkTexture(int index, uint32_t handle) { RWalkTexture_[index] = handle; }

	/// <summary>
	/// 左向きの停止中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetLIdleTexture(uint32_t handle) { LIdleTexture_ = handle; }

	/// <summary>
	/// 左向きの攻撃待機中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetLWaitTexture(uint32_t handle) { LWaitTexture_ = handle; }

	/// <summary>
	/// 左向きの攻撃中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetLAttackTexture(uint32_t handle) { LAttackTexture_ = handle; }

	/// <summary>
	/// 左向きのスタン中テクスチャハンドルを設定する
	/// </summary>
	/// <param name="handle">テクスチャハンドル</param>
	void SetLStunTexture(uint32_t handle) { LStunTexture_ = handle; }

	/// <summary>
	/// 左向きの歩行アニメーションの特定のテクスチャを設定する
	/// </summary>
	/// <param name="index">テクスチャ番号</param>
	/// <param name="handle">テクスチャハンドル</param>
	void SetLWalkTexture(int index, uint32_t handle) { LWalkTexture_[index] = handle; }

	/// <summary>
	/// 攻撃時のSEデータハンドルを設定留守
	/// </summary>
	/// <param name="handle">データハンドル</param>
	void SetAttackSEDataHandle(uint32_t handle) { attackSEDataHandle_ = handle; }

	// --- Virtual Callback ---

	/// <summary>
	/// 接触（被弾）時のコールバック関数
	/// </summary>
	/// <param name="damage">受けたダメージ量</param>
	/// <param name="attackDir">攻撃の飛んできた方向</param>
	virtual void OnHit(int32_t damage, const KamataEngine::Vector3& attackDir);

	/// <summary>
	/// 他の敵との距離を見て分離ベクトルを計算
	/// </summary>
	/// <param name="allEnemies">全敵リスト</param>
	/// <param name="separationDistance">最低距離</param>
	/// <returns>分離用オフセットベクトル</returns>
	KamataEngine::Vector3 ComputeSeparation(const std::vector<std::unique_ptr<EnemyBase>>& allEnemies, float separationDistance);

	/// <summary>
	/// 新しい状態へ切り替える
	/// </summary>
	/// <param name="newState">次に遷移する状態オブジェクト</param>
	void ChangeState(std::unique_ptr<BaseEnemyState> newState);

	// テンプレート版（ChangeState<EnemyStateIdle>() のように呼べて便利です）
	template<class T> void ChangeState() { ChangeState(std::make_unique<T>()); }

	/// <summary>
	/// 動きを止めるべき状態かどうかを判定する仮想関数
	/// </summary>
	/// <returns>基本ルール：ノックバック中、スタン中、死亡時は0を返す</returns>
	virtual bool IsMovementInterrupted() const;

protected:

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	/// <param name="allEnemies">全ての敵の情報</param>
	virtual void MoveTowardPlayer(const KamataEngine::Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies);

	/// <summary>
	/// 攻撃処理
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
	virtual void AttackProcess(const KamataEngine::Vector3& playerPos) = 0;


	// <summary>
	/// 基本的な近接攻撃の実行
	/// </summary>
	/// <param name="playerPos">攻撃対象であるプレイヤーの現在座標</param>
	void DoNormalAttack(const KamataEngine::Vector3& playerPos);

	/// <summary>
	/// テクスチャの更新
	/// </summary>
	virtual void UpdateTextures();



private:
	MyEngine::GameConfigManager* cfg_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	MyEngine::WorldTransformEx worldTransform_;
	MyEngine::WorldTransformEx worldTransformEHitBox_;
	MyEngine::WorldTransformEx worldTransformAHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelEHitBox_ = nullptr;
	KamataEngine::Model* modelAHitBox_ = nullptr;
	uint32_t textureHandle_ = 0;

	// 現在の状態を保持するスマートポインタ
	std::unique_ptr<BaseEnemyState> enemyState_;

	// ---- ステータス ----
	float speed_ = 0.0f;
	int32_t hp_ = 1;
	int32_t attackPower_ = 1;
	float facingDir_ = 1.0f;

	// ---- 移動 ----
	int32_t walkFrame_ = 0;      // 0〜3でループ
	int32_t walkFrameTimer_ = 0; // テクスチャ切替タイマー
	int32_t walkFrameInterval_;  // 何フレームごとに切り替えるか

	// ---- 攻撃 ----
	HitBox hitBox_;
	HitBox attackHitBox_;

	float attackCooldownTimer_ = 0;	
	float attackDuration_ ;
	float attackCooldown_;

	float attackTimer_ = 0.0f;
	bool isAttacking_ = false;
	bool isAttackMode_ = false;
	float attackRange_ = 1.0f;
	float attackDirX_ = 0.0f;

	bool hasDealtDamage_ = false;

	// ---- ノックバック ----
	bool isKnockBack_ = false;
	bool isDead_ = false;
	float knockbackTime_ = 0.0f;
	float knockbackDuration_;
	float knockbackHeight_;
	KamataEngine::Vector3 knockbackDir_;
	KamataEngine::Vector3 knockbackVelocity_{0, 0, 0};
	float gravity_;

	float deltaTime_ = 1.0f / 60.0f;

	// ---- スタン ----
	bool isStun_ = false;
	float stunTimer_ = 0.0f;
	float stunDuration_;
	float stunShakeTime_ = 0.0f;
	float stunShakeAmplitude_;
	float stunShakeSpeed_;
	KamataEngine::Vector3 originalPosition_ = {0.0f, 0.0f, 0.0f};

	// --- パーティクル用 ---
	std::unique_ptr<MyEngine::SmokeParticleManager> smokeManager_;
	float smokeSpawnTimer_ = 0.0f;
	float smokeSpawnInterval_;
	KamataEngine::Vector3 smokeSize_;
	std::unique_ptr<MyEngine::DustParticleManager> dustManager_;

	// ---- テクスチャ ----
	EnemyState state_ = EnemyState::Idle;

	uint32_t RIdleTexture_ = 0;
	uint32_t RWaitTexture_ = 0;
	uint32_t RAttackTexture_ = 0;
	uint32_t RStunTexture_ = 0;
	uint32_t RWalkTexture_[4];

	uint32_t LIdleTexture_ = 0;
	uint32_t LWaitTexture_ = 0;
	uint32_t LAttackTexture_ = 0;
	uint32_t LStunTexture_ = 0;
	uint32_t LWalkTexture_[4];

	// ---- SE ----
	uint32_t attackSEDataHandle_ = 0;
	uint32_t hitSEDataHandle_ = 0;
	uint32_t blownSEDataHandle_ = 0;

	uint32_t attackSEVoiceHandle_ = 0;
	uint32_t hitSEVoiceHandle_ = 0;
	uint32_t blownSEVoiceHandle_ = 0;

protected:

};
