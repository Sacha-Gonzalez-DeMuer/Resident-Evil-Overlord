#pragma once
#include "Components/BaseComponent.h"
#include "ResidentEvil/ReData.h"

namespace ZAnimNames {
	const std::wstring Idle{ L"Idle" };
	const std::wstring Walk{ L"Walk" };
	const std::wstring Dying{ L"Dying" };
	const std::wstring Attack{ L"Bite" };
}

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
	void TriggerState(const ZState& state);
	void TriggerState(const ZState& state, std::function<void()> onCompletion);
	void Pause() { m_pAnimator->Pause(); }
	const std::wstring& GetAnimName() const { return m_pAnimator->GetClipName(); }


private:
	ModelAnimator* m_pAnimator{};
	ReZombie* m_pZombie{};
};

