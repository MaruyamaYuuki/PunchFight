#pragma once
#include "DustParticle.h"
#include "ParticleManager.h"

namespace MyEngine {
/// <summary>
/// 地面から舞い上がる粉塵（砂埃）演出の一括生成および管理。
/// ランダム性を伴う大量生成 : Spawn 関数において、単一の座標から複数（5〜10個）の DustParticle をランダムな速度・サイズ・寿命で放出し、自然な拡散演出を作る。
/// リソース管理 : 粉塵専用のテクスチャ（dust2.png）とビルボードモデルの保持。
/// 描画制御 : 各パーティクルが持つ個別のアルファ値をモデルに反映し、カメラに対して一括描画を行う。
/// </summary>
class DustParticleManager : public ParticleManager {
public:
	/// <summary>
	/// 粉塵モデルの読み込みとテクスチャの設定。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 指定座標から複数の粉塵パーティクルをランダムな方向に放出する。
	/// </summary>
	/// <param name="pos">発生源となる中心座標</param>
	void Spawn(const KamataEngine::Vector3& pos);

	/// <summary>
	/// ビルボード形式で粉塵パーティクル群を描画する。
	/// </summary>
	/// <param name="camera">描画用カメラ</param>
	void Draw(KamataEngine::Camera& camera) override;

private:
	/// <summary>
	/// 指定範囲内からランダムな浮動小数点数を生成する。
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>生成されたランダム値</returns>
	float RandomRange(float min, float max);

    std::unique_ptr<KamataEngine::Model> model_;
	uint32_t textureHandle_ = 0;
};
} // namespace MyEngine