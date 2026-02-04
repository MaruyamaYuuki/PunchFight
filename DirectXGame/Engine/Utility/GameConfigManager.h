#pragma once
#include "KamataEngine.h" // Vector3などの型定義を含む
#include <map>
#include <string>
#include <vector>
#include <json.hpp>

namespace MyEngine {
/// <summary>
/// 外部設定ファイル（JSON）の一元管理と、プログラム各所へのパラメータ提供。
/// データ駆動の実現 : ゲームバランス調整に必要な数値（HP、攻撃力、座標など）を外部JSONファイルから読み込み、プログラムを再コンパイルすることなく調整可能な環境を提供する。
///データ構造の抽象化 : 複雑な階層を持つJSONデータを解析（パース）し、内部で std::map 等を用いてフラットなキー・バリュー形式として管理する。
/// 型安全なアクセス提供 : 内部で保持する文字列データを、利用側の要求に応じて int,float,Vector3,json_array 等の適切な型に変換して返却する。
/// グローバルなアクセシビリティ（シングルトン）: インスタンスを唯一のものに制限し、シーンを跨いで共通の設定値を参照できるようにする。
/// </summary>
class GameConfigManager {
private:
	std::map<std::string, std::string> configData_;

	// シングルトン化のためのコンストラクタ隠蔽
	GameConfigManager() = default;
	~GameConfigManager() = default;
	GameConfigManager(const GameConfigManager&) = delete;
	GameConfigManager& operator=(const GameConfigManager&) = delete;

private:
	/// <summary>
	/// JSONファイルをパースし、キーと値をconfigData_に格納する
	/// </summary>
	/// <param name="filePath">JSONファイルのパス</param>
	void LoadJson(const std::string& filePath);

public:
	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	static GameConfigManager* GetInstance();

	/// <summary>
	/// 初期化（JSONファイル読み込み）
	/// </summary>
	void Initialize(const std::string& filePath = "Resources/config/GameConfig.json");

	// --- データ取得用インターフェース ---

	/// <summary>
	/// 整数値を取得する
	/// </summary>
	int getInt(const std::string& key) const;

	/// <summary>
	/// 浮動小数点数値を取得する
	/// </summary>
	float getFloat(const std::string& key) const;

	/// <summary>
	/// 文字列を取得する
	/// </summary>
	std::string getString(const std::string& key) const;

	/// <summary>
	/// Vector3 (3要素の配列) を取得する
	/// </summary>
	KamataEngine::Vector3 getVector3(const std::string& key) const;

	/// <summary>
	/// Vector2 (2要素の配列) を取得する
	/// </summary>
	KamataEngine::Vector2 getVector2(const std::string& key) const;

	/// <summary>
	/// floatの配列を取得する（例: GameSceneのArea設定など）
	/// </summary>
	std::vector<float> getFloatArray(const std::string& key) const;

	/// <summary>
	/// JSON配列をそのまま取得する
	/// </summary>
	nlohmann::json getJsonArray(const std::string& key) const;

	/// <summary>
	/// JSONオブジェクトをそのまま取得する
	/// </summary>
	nlohmann::json getJsonObject(const std::string& key) const;
};
}
