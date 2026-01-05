#include "EnemyManager.h"
#include "../../../Engine/Math/Collision.h"
#include"../../../Engine/Utility//GameConfigManager.h"
#include <algorithm>
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void EnemyManager::Initialize() {
	enemies_.clear();
	areas_.clear();

    smokeManager_ = std::make_unique<SmokeParticleManager>();
	smokeManager_->Initialize();

	cfg_ = GameConfigManager::GetInstance();
}

void EnemyManager::AddArea(float triggerX) {
	EnemyArea area;
	area.triggerX = triggerX;
	areas_.push_back(area);
}

void EnemyManager::AddSpawnToArea(int32_t areaIndex, EnemyType type, const Vector3& pos) {
	if (areaIndex < 0 || areaIndex >= static_cast<int32_t>(areas_.size()))
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
		data = {cfg_->getString("Global.kCharacterModelPath"), 
			cfg_->getFloat("Enemy.Types.Normal.speed"), 
			cfg_->getInt("Enemy.Types.Normal.hp"), 
			cfg_->getInt("Enemy.Types.Normal.attackPower")
		};
		enemy = std::make_unique<NormalEnemy>();
		enemy->SetHitBox(pos, cfg_->getVector3("Enemy.Types.Normal.hitBoxSize")); // 中心0.5f、高さ1
		enemy->SetScale(cfg_->getVector3("Enemy.Types.Normal.scale"));
		break;
	case EnemyType::Power:
		data = {
			cfg_->getString("Global.kCharacterModelPath"), 
			cfg_->getFloat("Enemy.Types.Power.speed"), 
			cfg_->getInt("Enemy.Types.Power.hp"), 
			cfg_->getInt("Enemy.Types.Power.attackPower")
		};
		enemy = std::make_unique<PowerEnemy>();
		enemy->SetHitBox(pos, cfg_->getVector3("Enemy.Types.Power.hitBoxSize"));
		enemy->SetScale(cfg_->getVector3("Enemy.Types.Power.scale"));
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
		e->Update(playerPos, enemies_);
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
	smokeManager_->Draw(camera);
	for (auto& e : enemies_) {
		e->Draw(camera);
	}
}

void EnemyManager::BackDraw(KamataEngine::Camera& camera, const KamataEngine::Vector3& playerPos) {
	auto enemies = GetEnemiesSortedByZ(playerPos, true);

	for (auto* e : enemies) {
		e->Draw(camera);
	}
}

void EnemyManager::FrontDraw(KamataEngine::Camera& camera, const KamataEngine::Vector3& playerPos) {
	auto enemies = GetEnemiesSortedByZ(playerPos, false);

	for (auto* e : enemies) {
		e->Draw(camera);
	}
}

bool EnemyManager::IsAreaCleared(int32_t areaIndex) const {
	if (areaIndex < 0 || areaIndex >= static_cast<int32_t>(areas_.size()))
		return false;
	return areas_[areaIndex].cleared;
}

std::vector<EnemyBase*> EnemyManager::GetEnemiesSortedByZ(const KamataEngine::Vector3& playerPos, bool backSide) {
	std::vector<EnemyBase*> result;
	result.reserve(enemies_.size());

	for (auto& e : enemies_) {
		bool isBack = e->GetPosition().z > playerPos.z;

		if (isBack == backSide) {
			result.push_back(e.get());
		}
	}

	// 奥 → 手前
	std::sort(result.begin(), result.end(), [](EnemyBase* a, EnemyBase* b) { return a->GetPosition().z > b->GetPosition().z; });

	return result;
}