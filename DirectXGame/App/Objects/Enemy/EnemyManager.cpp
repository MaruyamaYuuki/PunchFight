#include "EnemyManager.h"
#include "../../../Engine/Math/Collision.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void EnemyManager::Initialize() {
	enemies_.clear();
	areas_.clear();
}

void EnemyManager::AddArea(float triggerX) {
	EnemyArea area;
	area.triggerX = triggerX;
	areas_.push_back(area);
}

void EnemyManager::AddSpawnToArea(int areaIndex, EnemyType type, const Vector3& pos) {
	if (areaIndex < 0 || areaIndex >= static_cast<int>(areas_.size()))
		return;

	EnemySpawnTrigger spawn;
	spawn.type = type;
	spawn.pos = pos;

	areas_[areaIndex].spawns.push_back(spawn);
}

void EnemyManager::SpawnEnemy(EnemyType type, const KamataEngine::Vector3& pos) {
	EnemyData data{};
	switch (type) {
	case EnemyType::Normal:
		data = {"boxFrame", 0.2f, 3, 5};
		break;
	case EnemyType::Power:
		data = {"enemy_power", 0.1f, 10, 10};
		break;
	}

std::unique_ptr<EnemyBase> enemy;

	switch (type) {
	case EnemyType::Normal:
		enemy = std::unique_ptr<EnemyBase>(new NormalEnemy());
		break;
	case EnemyType::Power:
		enemy = std::unique_ptr<EnemyBase>(new PowerEnemy());
		break;
	}


	enemy->Initialize(data);
	enemy->SetPosition(pos.x, pos.y, pos.z);

	enemies_.push_back(std::move(enemy));
}



void EnemyManager::Update(const Vector3& playerPos, const HitBox& playerAttackBox, int playerDamage) {
	// ======== エリアトリガーチェック ========
	for (auto& area : areas_) {

		if (!area.activated && playerPos.x >= area.triggerX) {
			area.activated = true;

			// --- 敵生成 ---
			for (auto& s : area.spawns) {
				SpawnEnemy(s.type, s.pos);
			}
		}
	}

// ======== 敵のアップデートと被弾判定 ========
	for (auto& e : enemies_) {
		e->Update(playerPos);

		if (playerAttackBox.active) {

			HitBox enemyBox;
			enemyBox.pos = e->GetPosition();
			enemyBox.size = {1, 1, 1};

			if (Collision::AABB(playerAttackBox, enemyBox)) {
				e->OnHit(playerDamage);
			}
		}
	}

	// ======== 死んだ敵を削除 ========
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](auto& e) { return e->IsDead(); }), enemies_.end());

	// ======== 各エリアの全滅判定 ========
	for (auto& area : areas_) {

		if (area.activated && !area.cleared) {

			bool allDead = true;

			for (auto& s : area.spawns) {
				// スポーン位置が一致する敵がまだ生存しているなら全滅ではない
				for (auto& e : enemies_) {
					// Vector3 に Length() がないので、明示的に距離を計算する
					const Vector3 posE = e->GetPosition();
					float dx = posE.x - s.pos.x;
					float dy = posE.y - s.pos.y;
					float dz = posE.z - s.pos.z;
					float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

					// スポーン位置とほぼ同じ座標（閾値0.5）にいる敵がいる -> 全滅ではない
					if (dist < 0.5f) {
						allDead = false;
						break;
					}
				}
				if (!allDead)
					break;
			}

			if (allDead) {
				area.cleared = true;
			}
		}
	}
}

void EnemyManager::Draw(Camera& camera) {
	for (auto& e : enemies_) {
		e->Draw(camera);
	}
}

bool EnemyManager::IsAreaCleared(int areaIndex) const {
	if (areaIndex < 0 || areaIndex >= static_cast<int>(areas_.size()))
		return false;
	return areas_[areaIndex].cleared;
}
