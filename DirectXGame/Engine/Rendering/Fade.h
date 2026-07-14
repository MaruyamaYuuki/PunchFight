#pragma once
#include "KamataEngine.h"

/// <summary>
/// シーン遷移や状態変化時における、画面全体の視覚的な平滑化（演出）。
/// 画面遮蔽の制御 : 全画面スプライトのアルファ値（透明度）を時間経過とともに変化させ、フェードイン・フェードアウトを実現する。
/// 時間管理 : duration（継続時間）に基づいた内部カウンターの更新と、演出終了判定（IsFinished）の提供。
/// 表示状態の抽象化 : Status（None,FadeIn,FadeOut等）によって現在の遷移状態をカプセル化し、外部から複雑な透明度計算を隠蔽する。
/// </summary>
namespace MyEngine{
class Fade {
public:
	enum class Status { None, FadeIn, FadeOut, AlphaFadeOut };

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Fade();

	/// <summary>
	/// フェード用スプライトの生成と初期化。初期状態は非表示(Alpha=0)に設定される。
	/// </summary>
	void Initialize();

	/// <summary>
	/// ステータスに応じたアルファ値の更新。Startで指定されたdurationに基づき、1フレームあたりの変化量を計算・適用する。
	/// </summary>
	void Update();

	/// <summary>
	/// 全画面を覆うフェード用スプライトの描画。
	/// </summary>
	void Draw();

	/// <summary>
	/// フェード演出の開始
	/// </summary>
	/// <param name="status">実行するフェードの種類（FadeIn/FadeOut/AlphaFadeOut）</param>
	/// <param name="duration">演出にかける時間（秒）</param>
	void Start(Status status, float duration);

	/// <summary>
	/// 現在進行中のフェード演出を強制停止し、状態を None に戻す。
	/// </summary>
	void Stop();

	/// <summary>
	/// フェードの終了判定
	/// </summary>
	/// <returns>指定した duration が経過し、演出が完了していれば true</returns>
	bool IsFinished() const;

	/// <summary>
	/// アルファ値（不透明度）を直接設定
	/// </summary>
	/// <param name="alpha">0.0f（透明） ～ 1.0f（不透明）の値</param>
	void SetAlpha(float alpha) { sprite_->SetColor({1, 1, 1, alpha}); }

private:
	uint32_t textureHandle_ = 0;
	std::unique_ptr<KamataEngine::Sprite> sprite_;

	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
	float scale_ = 0.0f;
	float alpha_ = 0.0f;
};
} // namespace MyEngine