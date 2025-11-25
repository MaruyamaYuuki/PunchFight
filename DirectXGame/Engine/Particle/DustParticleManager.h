#pragma once
#include "DustParticle.h"
#include "KamataEngine.h"

class DustParticleManager {
public:

	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	DustParticleManager() = default;

	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~DustParticleManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <param name="pos">生成する座標</param>
	void Spawn(const KamataEngine::Vector3& pos);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void Update(float deltaTime);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

	float RandomRange(float min, float max);

private:
	std::vector<std::unique_ptr<DustParticle>> particles_;

	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
