#pragma once
#include "Components/BaseComponent.h"


namespace PAnimNames {
	const std::wstring Idle{ L"Idle" };
	const std::wstring Walk{ L"Walk" };
	const std::wstring Sprint{ L"Running" };
	const std::wstring PistolIdle{ L"Pistol_Idle" };
	const std::wstring PistolWalk{ L"Pistol_Walk" };
	const std::wstring OpenDoor{ L"OpenDoor" };
	const std::wstring Bitten{ L"Bitten" };
}


// class that links player events to animations
class RePlayerController;
class ParticleEmitterComponent;
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
	void TriggerAnim(const std::wstring& clipName);
	void SetBloodEmitter(ParticleEmitterComponent* emitter) { m_pBloodEmitter = emitter; }

private:
	ModelAnimator* m_pAnimator{};
	RePlayerController* m_pPlayerController{};
	ParticleEmitterComponent* m_pBloodEmitter{};

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

 