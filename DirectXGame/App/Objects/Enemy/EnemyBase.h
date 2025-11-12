#pragma once
#include "../../../Engine/Math/Collider.h"
#include "../../../Engine/Math/WorldTransformEx.h"
#include "KamataEngine.h"

/// <summary>
/// 敵の情報
/// </summary>
struct EnemyData {
	std::string modelPath;
	float speed;
	int hp;
	float attackPower;
};

/// <summary>
/// 敵の状態
/// </summary>
enum class EnemyState { 
	Idle, 
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
	virtual void Update(const KamataEngine::Vector3& playerPos);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
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
	/// <returns></returns>
	const HitBox& GetHitBox() const { return hitBox_; }

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
	/// 敵の座標を取得する
	/// </summary>
	/// <returns></returns>
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

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
	/// テクスチャの更新
	/// </summary>
	void UpdateTextures();

protected:
	WorldTransformEx worldTransform_;
	WorldTransformEx worldTransformEHitBox_;
	WorldTransformEx worldTransformAHitBox_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelEHitBox_;
	KamataEngine::Model* modelAHitBox_;
	uint32_t textureHandle_ = 0;

	float speed_ = 0.0f;
	int hp_ = 1;
	float attackPower_ = 1.0f;
	float facingDir_ = 1.0f; // 向き（1.0f：右, -1.0f：左）
	int attackCooldownTimer_ = 0; // クールタイム残り時間
	const int attackDuration_ = 15; // 攻撃の長さ
	const int attackCooldown_ = 30;  // クールタイムの長さ
	/// ---ひるみ---
	bool isStan_ = false;
	float stanTimer_ = 0.0f;
	float stanDuration_ = 0.2f; // ひるみ時間

	HitBox hitBox_;
	HitBox attackHitBox_;

	float deltaTime = 1.0f / 60.0f;

    bool isDead_ = false;      // 完全に消えたか
    bool isKnockBack_ = false;
	float knockbackTime_ = 0.0f;
	float knockbackSpeed_ = 5.0f;    // X方向の吹っ飛び速度
	float knockbackDuration_ = 0.5f; // 0.5秒間吹っ飛ぶ
	float knockbackHeight_ = 2.0f;   // Y方向の少し浮く高さ
	KamataEngine::Vector3 knockbackDir_;
	KamataEngine::Vector3 knockbackVelocity_{0, 0, 0};
	float gravity_ = 30.0f;

	EnemyState state_ = EnemyState::Idle;
	uint32_t RIdleTexture_ = 0;
	uint32_t RAttackTexture_ = 0;
	uint32_t LIdleTexture_ = 0;
	uint32_t LAttackTexture_ = 0;
};
