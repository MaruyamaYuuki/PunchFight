#include "ParticleManager.h"

using namespace KamataEngine;

void ParticleManager::Update(float deltaTime) {
	for (auto& p : particles_) {
		if (p->active) {
			p->Update(deltaTime);
		}
	}
}
