#include "EnemyManager.h"

using namespace KamataEngine;

void EnemyManager::Initialize() { enemies_.clear(); }

void EnemyManager::SpawnEnemy(EnemyType type, const Vector3& pos) {
	EnemyData data{};
	switch (type) {
	case EnemyType::Normal:
		data = {"enemy_normal", 0.2f, 3, 5};
		break;
	case EnemyType::Power:
		data = {"enemy_power", 0.1f, 10, 10};
		break;
	}

	std::unique_ptr<EnemyBase> enemy;

	switch (type) {
	case EnemyType::Normal:
		enemy = std::make_unique<NormalEnemy>();
		break;
	case EnemyType::Power:
		enemy = std::make_unique<PowerEnemy>();
		break;
	}

	enemy->Initialize(data);
	enemy->SetPosition(pos.x, pos.y, pos.z);
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::Update(const Vector3& playerPos) {
	for (auto& e : enemies_) {
		e->Update(playerPos);
	}
}

void EnemyManager::Draw(Camera& camera) {
	for (auto& e : enemies_) {
		e->Draw(camera);
	}
}
