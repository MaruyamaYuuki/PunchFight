#pragma once
#include <memory>
#include "../../../Engine/Math/Collider.h"
#include "../../../Engine/Math/WorldTransformEx.h"
#include "../../../Engine/Particle/SmokeParticle.h"
#include "../../../Engine/Particle/SmokeParticleManager.h"
#include "../../../Engine//Particle/DustParticleManager.h"
#include "KamataEngine.h"

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

	/// <summary>
	/// 接触判定
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="attackDir">攻撃の方向	</param>
	virtual void OnHit(int32_t damage, const KamataEngine::Vector3& attackDir);

	/// <summary>
	/// 敵の死亡判定
	/// </summary>
	/// <returns>死亡していれば true、生存していれば false</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 敵のヒットボックスを取得する
	/// </summary>
	/// <returns>当たり判定の情報（位置・サイズ・有効状態など）</returns>
	const HitBox& GetHitBox() const { return hitBox_; }

	/// <summary>
	/// 敵の攻撃のヒットボックスを取得する
	/// </summary>
	/// <returns>攻撃判定の情報（位置・サイズ・有効状態など）</returns>
	const HitBox& GetAttackHitBox() const { return attackHitBox_; }

	/// <summary>
	/// 攻撃力を取得する
	/// </summary>
	/// <returns>敵の攻撃力</returns>
	int32_t GetAttackPower() const { return attackPower_; }

    /// <summary>
	/// 敵の座標を取得する
	/// </summary>
	/// <returns>現在のワールド座標（translation）</returns>
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	/// <summary>
	/// 敵のHPを取得する
	/// </summary>
	/// <returns>現在のヒットポイント残り合計値</returns>
	int32_t GetHP() const { return hp_; }

	/// <summary>
	/// ノックバック判定
	/// </summary>
	/// <returns>ノックバックしていれば true、していなければ false</returns>
	bool IsKnockBack() const { return isKnockBack_; }

	/// <summary>
	/// 攻撃中か判定
	/// </summary>
	/// <returns>攻撃中なら true、してなければ false</returns>
	bool IsAttacking() const { return attackHitBox_.active; }

	/// <summary>
	/// ダメージを与えるか判定
	/// </summary>
	/// <returns>与えるなら true、与えないなら false</returns>
	bool HasDealtDamage() const { return hasDealtDamage_; }

	/// <summary>
	/// 敵の座標を設定する
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <param name="z">Z座標</param>
	void SetPosition(float x, float y, float z);

	/// <summary>
	/// 敵の大きさを設定する
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetScale(const KamataEngine::Vector3& scale);

	/// <summary>
	/// 敵のヒットボックスを設定する
	/// </summary>
	/// <param name="center">中心座標</param>
	/// <param name="size">大きさ</param>
	void SetHitBox(const KamataEngine::Vector3& center, const KamataEngine::Vector3& size);

	/// <summary>
	/// 攻撃のヒットボックスを設定する
	/// </summary>
	/// <param name="pos">座標</param>
	void SetAttackHitBox(const KamataEngine::Vector3& pos);

	/// <summary>
	/// ダメージを与えるか設定
	/// </summary>
	/// <param name="flag">フラグ</param>
	void SetHasDealtDamage(bool flag) { hasDealtDamage_ = flag; }

	/// <summary>
	/// 他の敵との距離を見て分離ベクトルを計算
	/// </summary>
	/// <param name="allEnemies">全敵リスト</param>
	/// <param name="separationDistance">最低距離</param>
	/// <returns>分離用オフセットベクトル</returns>
	KamataEngine::Vector3 ComputeSeparation(const std::vector<std::unique_ptr<EnemyBase>>& allEnemies, float separationDistance);

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

	/// <summary>
	/// 動きを止めるべき状態かどうかを判定する仮想関数
	/// </summary>
	/// <returns>基本ルール：ノックバック中、スタン中、死亡時は0を返す</returns>
	virtual bool IsMovementInterrupted() const;

	/// <summary>
	/// 状態を自動で更新するヘルパー関数
	/// </summary>
	void UpdateBasicState();

protected:
	MyEngine::GameConfigManager* cfg_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	MyEngine::WorldTransformEx worldTransform_;
	MyEngine::WorldTransformEx worldTransformEHitBox_;
	MyEngine::WorldTransformEx worldTransformAHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelEHitBox_ = nullptr;
	KamataEngine::Model* modelAHitBox_ = nullptr;
	uint32_t textureHandle_ = 0;

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
	float ATTACK_RANGE_ = 1.0f;
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

private:
	// --- パーティクル用 ---
	std::unique_ptr<MyEngine::SmokeParticleManager> smokeManager_;
	float smokeSpawnTimer_ = 0.0f;
	float smokeSpawnInterval_;
	KamataEngine::Vector3 smokeSize_;
	std::unique_ptr<MyEngine::DustParticleManager> dustManager_;

	// ---- スタン ----
	float stunTimer_ = 0.0f;
	float stunDuration_;
	float stunShakeTime_ = 0.0f;
	float stunShakeAmplitude_;
	float stunShakeSpeed_;
	KamataEngine::Vector3 originalPosition_ = {0.0f, 0.0f, 0.0f};
};
