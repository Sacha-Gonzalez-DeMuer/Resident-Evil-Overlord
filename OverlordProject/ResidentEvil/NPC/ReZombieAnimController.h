#pragma once
#include "Components/BaseComponent.h"

// class that links player events to animations
class ReZombie;
class ReZombieAnimController final : public BaseComponent
{
public:
	ReZombieAnimController(ModelAnimator* modelAnimator, ReZombie* zombie);
	~ReZombieAnimController() override = default;
	ReZombieAnimController(const ReZombieAnimController& other) = delete;
	ReZombieAnimController(ReZombieAnimController&& other) noexcept = delete;
	ReZombieAnimController& operator=(const ReZombieAnimController& other) = delete;
	ReZombieAnimController& operator=(ReZombieAnimController&& other) noexcept = delete;
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	ModelAnimator* m_pAnimator{};
	ReZombie* m_pZombie{};
};

