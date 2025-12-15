#include "PowerEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void PowerEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	RIdleTexture_ = TextureManager::Load("enemies/powerEnemy/RPower.png");
	RAttackTexture_ = TextureManager::Load("enemies/powerEnemy/RHeadbutt1.png");
	RStunTexture_ = TextureManager::Load("enemies/powerEnemy/RStun.png");

	LIdleTexture_ = TextureManager::Load("enemies/powerEnemy/LPower.png");
	LAttackTexture_ = TextureManager::Load("enemies/powerEnemy/LHeadbutt1.png");
	LStunTexture_ = TextureManager::Load("enemies/powerEnemy/LStun.png");
}

void PowerEnemy::Update(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {
	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	worldTransform_.UpdateMatrix();
}
