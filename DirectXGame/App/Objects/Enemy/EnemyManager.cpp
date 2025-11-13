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
	std::unique_ptr<EnemyBase> enemy;

	switch (type) {
	case EnemyType::Normal:
		data = {"enemy", 0.1f, 35, 5};
		enemy = std::make_unique<NormalEnemy>();
		enemy->SetHitBox(pos, {0.5f, 1.0f, 0.025f}); // 中心0.5f、高さ1
		enemy->SetScale({0.5f, 0.5f, 0.5f});
		break;
	case EnemyType::Power:
		data = {"enemy", 0.1f, 30, 10};
		enemy = std::make_unique<PowerEnemy>();
		enemy->SetHitBox(pos, {1.0f, 2.0f, 1.0f});
		enemy->SetScale({2.0f, 2.0f, 2.0f});
		break;
	}

	enemy->Initialize(data);
	enemy->SetPosition(pos.x, pos.y, pos.z);

	enemies_.push_back(std::move(enemy));
}

void EnemyManager::Update(const Vector3& playerPos) {
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
		DebugText::GetInstance()->ConsolePrintf("HP : %d\nIsStun : %d\nState : %d\n" ,e->GetHP(),e->IsStun(),e->GetState());
	}

	// ======== 死んだ敵を削除 ========
	enemies_.erase(
	    std::remove_if(enemies_.begin(), enemies_.end(), [](auto& e) { return e->IsDead(); }), // knockback終了後に消す
	    enemies_.end());


	// ======== 各エリアの全滅判定 ========
	for (auto& area : areas_) {

		if (area.activated && !area.cleared) {

			bool allDead = true;

			// area内のスポーンした敵がまだ生存しているか確認
			for (auto& e : enemies_) {
				// HP > 0 または knockBack中なら生存とみなす
				if (!e->IsDead() && e->GetHP() >= 0) {
					allDead = false;
					break;
				}
			}
			// 生存中の敵がいなければ全滅
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

void EnemyManager::BackDraw(KamataEngine::Camera& camera, const KamataEngine::Vector3& playerPos) {
	for (auto& e : enemies_) {
		if (e->GetPosition().z > playerPos.z) {
    		e->Draw(camera);
		}
	}
}

void EnemyManager::FrontDraw(KamataEngine::Camera& camera, const KamataEngine::Vector3& playerPos) {
	for (auto& e : enemies_) {
		if (e->GetPosition().z <= playerPos.z) {
			e->Draw(camera);
		}
	}
}

bool EnemyManager::IsAreaCleared(int areaIndex) const {
	if (areaIndex < 0 || areaIndex >= static_cast<int>(areas_.size()))
		return false;
	return areas_[areaIndex].cleared;
}