#pragma once

#include "EnemyBase.h"
#include "NormalEnemy.h"
#include "PowerEnemy.h"
#include <memory> // unique_ptr
#include <vector>

enum class EnemyType { Normal, Power };

struct EnemySpawnTrigger {
	EnemyType type;
	KamataEngine::Vector3 pos;
};

struct EnemyArea {
	float triggerX;
	std::vector<EnemySpawnTrigger> spawns;
	bool activated = false;
	bool cleared = false;
};

class EnemyManager {
public:
	void Initialize();
	void Update(const KamataEngine::Vector3& playerPos, const HitBox& playerAttackBox, int playerDamage);
	void Draw(KamataEngine::Camera& camera);
	void AddArea(float triggerX);
	void AddSpawnToArea(int areaIndex, EnemyType type, const KamataEngine::Vector3& pos);
	bool IsAreaCleared(int areaIndex) const;
	const std::vector<std::unique_ptr<EnemyBase>>& GetEnemies() const { return enemies_; }

private:
	void SpawnEnemy(EnemyType type, const KamataEngine::Vector3& pos);

	std::vector<std::unique_ptr<EnemyBase>> enemies_;
	std::vector<EnemyArea> areas_;

	bool isHit_ = false;
};
