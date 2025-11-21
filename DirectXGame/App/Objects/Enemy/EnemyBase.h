#pragma once
#include <memory>
#include "../../../Engine/Math/Collider.h"
#include "../../../Engine/Math/WorldTransformEx.h"
#include "../../../Engine/Particle/SmokeParticle.h"
#include "../../../Engine/Particle/SmokeParticleManager.h"
#include "KamataEngine.h"

/// <summary>
/// 敵の情報
/// </summary>
struct EnemyData {
	std::string modelPath;
	float speed;
	int hp;
	int attackPower;
};

/// <summary>
/// 敵の状態
/// </summary>
enum class EnemyState { 
	Idle = 0, 
	Walking, 
	Attacking, 
	Stunned, 
	Knockback, 
	Dead 
};

/// <summary>
/// 敵の基盤
/// </summary>
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

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="playerPos">プレイヤーの座標</param>
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
	virtual void OnHit(int damage, const KamataEngine::Vector3& attackDir);

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
	int GetAttackPower() const { return attackPower_; }

	/// <summary>
	/// 敵の座標を取得する
	/// </summary>
	/// <returns></returns>
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	/// <summary>
	/// 敵のHPを取得する
	/// </summary>
	/// <returns></returns>
	int GetHP() const { return hp_; }

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
	/// テクスチャの更新
	/// </summary>
	void UpdateTextures();

	/// <summary>
	/// 他の敵との距離を見て分離ベクトルを計算
	/// </summary>
	/// <param name="allEnemies">全敵リスト</param>
	/// <param name="separationDistance">最低距離</param>
	/// <returns>分離用オフセットベクトル</returns>
	KamataEngine::Vector3 ComputeSeparation(const std::vector<std::unique_ptr<EnemyBase>>& allEnemies, float separationDistance);

protected:
	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformEHitBox_;
	WorldTransformEx worldTransformAHitBox_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelEHitBox_ = nullptr;
	KamataEngine::Model* modelAHitBox_ = nullptr;
	uint32_t textureHandle_ = 0;

	// ---- ステータス ----
	float speed_ = 0.0f;
	int hp_ = 1;
	int attackPower_ = 1;
	float facingDir_ = 1.0f;

	// ---- 攻撃 ----
	HitBox hitBox_;
	HitBox attackHitBox_;

	float attackCooldownTimer_ = 0;
	const float attackDuration_ = 1.0f;
	const float attackCooldown_ = 3.0f;

	bool hasDealtDamage_ = false;

	// ---- スタン ----
	bool isStun_ = false;
	float stunTimer_ = 0.0f;
	float stunDuration_ = 1.0f;

	// ---- ノックバック ----
	bool isKnockBack_ = false;
	bool isDead_ = false;
	float knockbackTime_ = 0.0f;
	float knockbackSpeed_ = 5.0f;
	float knockbackDuration_ = 0.5f;
	float knockbackHeight_ = 2.0f;
	KamataEngine::Vector3 knockbackDir_;
	KamataEngine::Vector3 knockbackVelocity_{0, 0, 0};
	float gravity_ = 30.0f;

	float deltaTime = 1.0f / 60.0f;

    // --- パーティクル用 ---
	std::unique_ptr<SmokeParticleManager> smokeManager_;
	float smokeSpawnTimer_ = 0.0f;
	const float smokeSpawnInterval_ = 0.05f;

	// ---- テクスチャ ----
	EnemyState state_ = EnemyState::Idle;

	uint32_t RIdleTexture_ = 0;
	uint32_t RAttackTexture_ = 0;
	uint32_t RStunTexture_ = 0;

	uint32_t LIdleTexture_ = 0;
	uint32_t LAttackTexture_ = 0;
	uint32_t LStunTexture_ = 0;
};
