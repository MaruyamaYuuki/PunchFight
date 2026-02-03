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
	/// 初期化
	/// </summary>
	/// <param name="stageNumber">ステージの番号</param>
	/// <param name="repeatCount">生成するステージモデルの個数</param>
	void Initialize(int32_t stageNumber, int32_t repeatCount);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="cameraX">カメラのX座標</param>
	void Update(float cameraX);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(KamataEngine::Camera& camera);

private:
	/// <summary>
	/// オブジェクトの読み込み
	/// </summary>
	void UpdateLoadedStages();

private:
	MyEngine::GameConfigManager* cfg_ = nullptr;

	std::unordered_map<int32_t, std::unique_ptr<Stage>> activeStages_;

	KamataEngine::Model* startModel_ = nullptr;
	KamataEngine::Model* midModel_ = nullptr;
	KamataEngine::Model* endModel_ = nullptr;

	float stageWidth_; // 区間の幅
	int32_t currentIndex_ = 0;    // 現在位置の区間インデックス
	int32_t stageCount_ = 0;      // ステージ全体の区間数
};
