#pragma once
#include "Components/BaseComponent.h"

// class that links player events to animations
class RePlayerController;
class RePlayerAnimController final : public BaseComponent
{
public:
	RePlayerAnimController(ModelAnimator* modelAnimator, RePlayerController* player);
	~RePlayerAnimController() override = default;
	RePlayerAnimController(const RePlayerAnimController& other) = delete;
	RePlayerAnimController(RePlayerAnimController&& other) noexcept = delete;
	RePlayerAnimController& operator=(const RePlayerAnimController& other) = delete;
	RePlayerAnimController& operator=(RePlayerAnimController&& other) noexcept = delete;
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	ModelAnimator* m_pAnimator{};
	RePlayerController* m_pPlayerController{};

	enum class AnimState
	{
		Idle,
		Walk,
		Run,
		Aim,
		Attack,
		Bitten,
		Dead
	};
};

 