#pragma once
#include "KamataEngine.h"
#include "Stage.h"
#include <deque>

/// <summary>
/// Stage オブジェクトを組み合わせた、ゲーム全体のフィールド構築と動的制御。
/// ステージの構成データ（区間数やモデルの種類）の初期化。
/// カメラの進行状況（cameraX）に応じた、ステージの動的な更新。
/// std::unordered_map 等を用いた、アクティブなステージセクションの効率的な管理。
// スタート地点、中間地点、エンド地点といった異なる役割を持つモデルの適切な配置順序の制御。
/// </summary>
namespace MyEngine {
    class GameConfigManager;
}
class StageManager {
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	StageManager() = default;

	/// <summary>
	/// デフォルトデストラクタ
	/// </summary>
	~StageManager() = default;

	/// <summary>
	/// ステージ全体の初期化とモデルの読み込み
	/// </summary>
	/// <param name="stageNumber">読み込むステージの番号（1:草原, 2:洞窟 など）</param>
	/// <param name="repeatCount">中間セクション（midModel）を生成する総数</param>
	void Initialize(int32_t stageNumber, int32_t repeatCount);

	/// <summary>
	/// カメラ位置に基づいたステージの動的生成・更新
	/// </summary>
	/// <param name="cameraX">現在のメインカメラのX座標（これに基づき表示範囲を判定）</param>
	void Update(float cameraX);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

private:
	/// <summary>
	/// カメラの進行状況に合わせ、必要なステージ区間の生成と不要な区間の破棄を実行
	/// </summary>
	void UpdateLoadedStages();

private:
	MyEngine::GameConfigManager* cfg_ = nullptr;

	std::unordered_map<int32_t, std::unique_ptr<Stage>> activeStages_;

std::unique_ptr<KamataEngine::Model> startModel_;
	std::unique_ptr<KamataEngine::Model> midModel_;
	std::unique_ptr<KamataEngine::Model> endModel_;

	float stageWidth_; // 区間の幅
	int32_t currentIndex_ = 0;    // 現在位置の区間インデックス
	int32_t stageCount_ = 0;      // ステージ全体の区間数
};
