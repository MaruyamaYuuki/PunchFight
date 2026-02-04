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
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フェード演出の開始
	/// </summary>
	/// <param name="status">実行するフェードの種類（FadeIn/FadeOut/AlphaFadeOut）</param>
	/// <param name="duration">演出にかける時間（秒）</param>
	void Start(Status status, float duration);

	/// <summary>
	/// フェードの終了処理
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
	KamataEngine::Sprite* sprite_ = nullptr;

	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;
	float scale_ = 0.0f;
	float alpha_ = 0.0f;
};
} // namespace MyEngine