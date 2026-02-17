#pragma once
#include "ParticleManager.h"
#include "SmokeParticle.h"

namespace MyEngine {
/// <summary>
/// 衝撃や爆発に伴う煙エフェクトの生成および管理。
/// 特定サイズのアニメーション開始 : Spawn 関数において、呼び出し側から指定されたサイズ（size）を初期値として SmokeParticle を生成し、膨張アニメーションの起点を制御する。
/// 前後関係の微調整 : 煙が他のオブジェクトと重なった際の描画順を考慮し、Z座標（translation_.z）を微調整して配置する。
/// 柔軟な外見変更 : SetTexture インターフェースを提供し、状況に応じて異なる種類の煙を使い分けられるようにする。
/// </summary>
class SmokeParticleManager : public ParticleManager {
public:
	/// <summary>
	/// 煙モデルの読み込みと初期設定。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 指定座標に、指定された初期サイズで煙パーティクルを生成する。
	/// </summary>
	/// <param name="pos">発生座標</param>
	/// <param name="size">初期スケール（ここから時間経過で膨張する）</param>
	void Spawn(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& size);

	/// <summary>
	/// 煙パーティクル群を描画する。
	/// </summary>
	/// <param name="camera">描画用カメラ</param>
	void Draw(KamataEngine::Camera& camera) override;

	/// <summary>
	/// TextureHandleを設定する
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetTexture(uint32_t textureHandle) { textureHandle_ = textureHandle; }

private:
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;
};
} // namespace MyEngine