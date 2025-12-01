#pragma once
#include "KamataEngine.h"
#include "SmokeParticle.h"

class SmokeParticleManager {
public:

	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	SmokeParticleManager() = default;

	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~SmokeParticleManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <param name="pos">生成する座標</param>
	void Spawn(const KamataEngine::Vector3& pos, const KamataEngine::Vector3 size);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void Update(float deltaTime);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

	/// <summary>
	/// TextureHandleを設定する
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetTexture(uint32_t textureHandle) { textureHandle_ = textureHandle; }

private:
	std::vector<std::unique_ptr<SmokeParticle>> particles_;

	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
