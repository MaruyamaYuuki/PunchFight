#pragma once
#include "NormalEnemy.h"
#include "PowerEnemy.h"

enum class EnemyType {
	Normal,
	Power,
};

class EnemyManager {
	void Initialize();

	void Update(const KamataEngine::Vector3& playerPos);

	void Draw(KamataEngine::Camera& camera);

	void SpawnEnemy(EnemyType type, const KamataEngine::Vector3& pos);

private:
	std::vector<std::unique_ptr<EnemyBase>> enemies_;
};
