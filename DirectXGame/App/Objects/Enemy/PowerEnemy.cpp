#include "PowerEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void PowerEnemy::Initialize(const EnemyData& data) { EnemyBase::Initialize(data); }

void PowerEnemy::Update(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {
	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	worldTransform_.UpdateMatrix();
}
