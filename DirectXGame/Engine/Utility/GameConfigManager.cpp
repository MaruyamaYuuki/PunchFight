#include "GameConfigManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

/// <summary>
/// JSONオブジェクトを再帰的にトラバースし、階層構造をフラットなキー-値マップに変換する
/// </summary>
/// <param name="j">現在のJSONノード</param>
/// <param name="currentKey">現在のキーパス（例: "Player.Attack"）</param>
/// <param name="outputMap">結果を格納するマップ</param>
void FlattenJson(const json& j, const std::string& currentKey, std::map<std::string, std::string>& outputMap) {
	for (json::const_iterator it = j.begin(); it != j.end(); ++it) {
		std::string newKey = currentKey.empty() ? it.key() : currentKey + "." + it.key();

		if (it->is_object()) {
			// オブジェクトの場合、再帰的に処理
			FlattenJson(*it, newKey, outputMap);
		} else if (it->is_array()) {
			// 配列の場合、配列全体を文字列として保存 (Vector3やFloatArray用)
			// 配列の文字列表現（例: "[1.0, 2.0, 3.0]"）をマップに保存
			outputMap[newKey] = it->dump();
		} else {
			// 単一の値（int, float, string, boolなど）の場合、文字列として保存
			outputMap[newKey] = it->dump();
		}
	}
}

GameConfigManager* GameConfigManager::GetInstance() {
	static GameConfigManager instance;
	return &instance;
}

void GameConfigManager::LoadJson(const std::string& filePath) {
	std::ifstream ifs(filePath);
	if (!ifs.is_open()) {
		std::cerr << "Error: Failed to open config file: " << filePath << std::endl;
		// ファイルが開けない場合、続行できないため例外を投げるか、デフォルト値を設定する
		return;
	}

	try {
		json j;
		ifs >> j;

		configData_.clear();
		FlattenJson(j, "", configData_);

		std::cout << "GameConfigManager: Successfully loaded " << configData_.size() << " settings from " << filePath << std::endl;

	} catch (const json::parse_error& e) {
		std::cerr << "Error: JSON parsing failed in " << filePath << ": " << e.what() << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error during JSON loading: " << e.what() << std::endl;
	}
}

void GameConfigManager::Initialize(const std::string& filePath) { LoadJson(filePath); }

namespace {
// 汎用的なマップ検索ヘルパー
const std::string& FindConfigValue(const std::map<std::string, std::string>& data, const std::string& key) {
	auto it = data.find(key);
	if (it == data.end()) {
		std::string errMsg = "Config key not found: " + key;
		throw std::runtime_error(errMsg.c_str());
	}
	// JSONのdump()で保存された文字列は、クォーテーションで囲まれている場合があるため、
	// 文字列型として取得する場合以外は、ここでクォーテーションを取り除く処理を入れることが多いが、
	// 以下の std::stoi/stof で対応可能な場合は不要とする。
	return it->second;
}
} // namespace

int GameConfigManager::getInt(const std::string& key) const {
	const std::string& s = FindConfigValue(configData_, key);
	try {
		// 文字列からクォーテーションを削除 (整数/浮動小数点数以外の型でgetStringを呼ぶ際に必要となる場合がある)
		std::string cleaned_s = s;
		cleaned_s.erase(std::remove(cleaned_s.begin(), cleaned_s.end(), '"'), cleaned_s.end());

		return std::stoi(cleaned_s);
	} catch (const std::exception& e) {
		std::string errMsg = "Config type conversion failed (int) for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}

float GameConfigManager::getFloat(const std::string& key) const {
	const std::string& s = FindConfigValue(configData_, key);
	try {
		// 文字列からクォーテーションを削除
		std::string cleaned_s = s;
		cleaned_s.erase(std::remove(cleaned_s.begin(), cleaned_s.end(), '"'), cleaned_s.end());

		return std::stof(cleaned_s);
	} catch (const std::exception& e) {
		std::string errMsg = "Config type conversion failed (float) for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}

std::string GameConfigManager::getString(const std::string& key) const {
	const std::string& s = FindConfigValue(configData_, key);
	// JSONのdump()により、文字列は通常 "value" の形で保存されている。
	// そのため、両端のクォーテーションを取り除く。
	if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
		return s.substr(1, s.length() - 2);
	}
	// 文字列以外の値が保存されている場合（例: "5.0", "10"）
	return s;
}

KamataEngine::Vector3 GameConfigManager::getVector3(const std::string& key) const {
	// 値は "[x, y, z]" の形で文字列として保存されている
	std::string s = FindConfigValue(configData_, key);

	try {
		// nlohmann::json の from_string を使ってパースし直す
		json j_array = json::parse(s);

		if (j_array.is_array() && j_array.size() == 3) {
			return KamataEngine::Vector3{j_array[0].get<float>(), j_array[1].get<float>(), j_array[2].get<float>()};
		}
		throw std::runtime_error("Array size is not 3 or not an array.");
	} catch (const json::parse_error& e) {
		std::string errMsg = "Config Vector3 parsing failed for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	} catch (const std::exception& e) {
		std::string errMsg = "Config type conversion failed (Vector3) for key: " + key + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}

KamataEngine::Vector2 GameConfigManager::getVector2(const std::string& key) const { 	
	// 値は "[x, y]" の形で文字列として保存されている
	std::string s = FindConfigValue(configData_, key);

	try {
		// nlohmann::json の from_string を使ってパースし直す
		json j_array = json::parse(s);

		if (j_array.is_array() && j_array.size() == 2) {
			return KamataEngine::Vector2{j_array[0].get<float>(), j_array[1].get<float>()};
		}
		throw std::runtime_error("Array size is not 2 or not an array.");
	} catch (const json::parse_error& e) {
		std::string errMsg = "Config Vector2 parsing failed for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	} catch (const std::exception& e) {
		std::string errMsg = "Config type conversion failed (Vector2) for key: " + key + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}

std::vector<float> GameConfigManager::getFloatArray(const std::string& key) const {
	// 値は "[f1, f2, f3, ...]" の形で文字列として保存されている
	std::string s = FindConfigValue(configData_, key);

	try {
		// nlohmann::json の from_string を使ってパースし直す
		json j_array = json::parse(s);

		if (j_array.is_array()) {
			std::vector<float> result;
			for (const auto& element : j_array) {
				// 配列の各要素をfloatに変換して格納
				result.push_back(element.get<float>());
			}
			return result;
		}
		throw std::runtime_error("Value is not an array.");
	} catch (const json::parse_error& e) {
		std::string errMsg = "Config FloatArray parsing failed for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	} catch (const std::exception& e) {
		std::string errMsg = "Config type conversion failed (FloatArray) for key: " + key + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}

nlohmann::json GameConfigManager::getJsonArray(const std::string& key) const {
	const std::string& s = FindConfigValue(configData_, key);

	try {
		nlohmann::json j = nlohmann::json::parse(s);

		if (!j.is_array()) {
			throw std::runtime_error("Value is not a JSON array.");
		}
		return j;
	} catch (const std::exception& e) {
		std::string errMsg = "Config JSON array parsing failed for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}

nlohmann::json GameConfigManager::getJsonObject(const std::string& key) const {
	const std::string& s = FindConfigValue(configData_, key);

	try {
		nlohmann::json j = nlohmann::json::parse(s);

		if (!j.is_object()) {
			throw std::runtime_error("Value is not a JSON object.");
		}
		return j;
	} catch (const std::exception& e) {
		std::string errMsg = "Config JSON object parsing failed for key: " + key + ". Value: " + s + ". Error: " + e.what();
		throw std::runtime_error(errMsg.c_str());
	}
}
