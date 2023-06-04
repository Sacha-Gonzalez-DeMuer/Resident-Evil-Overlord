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
	const std::wstring Death{ L"Death" };
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
	void TriggerAnim(const std::wstring& clipName, const std::wstring& nextClipName, std::function<void()> callback);
	void SetBloodEmitter(ParticleEmitterComponent* emitter) { m_pBloodEmitter = emitter; }
	void ClearNextAnim();
	void Pause();
	bool IsPaused() const { return m_Pause; }
	void PausePlayerAnimator() { m_Pause = true; }
	void Reset();
private:
	ModelAnimator* m_pAnimator{};
	RePlayerController* m_pPlayerController{};
	ParticleEmitterComponent* m_pBloodEmitter{};
	float m_BloodDelay{ 1.f };
	float m_BloodTimer{};
	// death callback wouldn't work so doing it this way..
	float m_DeathAnimTime{2.83f}; 
	float m_DeathTimer{ 2.83f };
	bool m_Pause{ false };

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

 