#pragma once
#include "Particle.h"

namespace MyEngine {
 /// <summary>
/// 爆発や衝撃時に発生する、時間経過で膨らむ「煙」の演出実装。
/// 動的なスケール変化の制御 : 初期サイズ（initialScale）から終了時間に向けて徐々に拡大していく膨張アニメーションの実行。
/// 煙特有の浮遊感の演出 : 非常に緩やかな上昇移動とフェードアウトを組み合わせ、空気中に留まるような視覚効果の維持。
/// </summary>
class SmokeParticle : public Particle {
public:
	KamataEngine::Vector3 initialScale;

	void Update(float deltaTime) override;
};
}
