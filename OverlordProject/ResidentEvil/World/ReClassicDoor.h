#pragma once
#include "Delegate.h"

class ReClassicDoor final : public GameObject
{
	public:
	ReClassicDoor();
	ReClassicDoor(float duration, float startYaw, float endYaw);
	~ReClassicDoor() override = default;
	ReClassicDoor(const ReClassicDoor& other) = delete;
	ReClassicDoor(ReClassicDoor&& other) noexcept = delete;
	ReClassicDoor& operator=(const ReClassicDoor& other) = delete;
	ReClassicDoor& operator=(ReClassicDoor&& other) noexcept = delete;
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
	void Trigger();
	void SetSceneToLoad(const std::wstring& scene) { m_SceneToLoad = scene; }

	Delegate<> OnAnimationFinished;
private:
	void UpdateKeyframeEvents();

	bool m_TriggerOpen{};
	float m_Duration{};
	float m_TotalYaw{};
	float m_StartYaw{};
	float m_EndYaw{};
	float m_AnimLerp{};
	float m_TimePassed{};
	int m_CamID{-1};
	std::wstring m_SceneToLoad{ };

	// keyframes
	float m_DoorHingeSFXtime{ 2.f };
	bool m_DoorHingeSFXplayed{ false };

	float m_DoorOpenTime{ 3.f };
	bool m_OpenDoor{ false };

	float m_DoorFullOpenTime{ 4.f };

	float m_DoorCloseSFXtime{ 5.5f };
	bool m_DoorCloseSFXplayed{ false };
};

