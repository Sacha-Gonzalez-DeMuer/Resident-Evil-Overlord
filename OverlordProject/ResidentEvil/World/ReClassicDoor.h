#pragma once
#include "Delegate.h"

class ThunderController;
class ReCamera;
enum class ReScenes;
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
	void SetSceneToLoad(const ReScenes& scene);

	Delegate<> OnAnimationStart;
	Delegate<> OnAnimationFinished;

	void Reset();
	void ClearDelegates();

private:
	void UpdateKeyframeEvents();
	ReScenes m_SceneToLoad{};
	ReCamera* m_pCamera{};

	bool m_TriggerOpen{};
	float m_Duration{};
	float m_TotalYaw{};
	float m_StartYaw{};
	float m_EndYaw{};
	float m_TimePassed{};
	int m_CamID{-1};
	bool m_AnimationOnly{ false };
	// keyframes
	float m_StartMoveCamTime{ 2.f };
	bool m_CamMoved{ false };

	float m_DoorOpenTime{ 3.f };
	bool m_OpenDoor{ false };

	float m_DoorFullOpenTime{ 4.f };

	float m_DoorCloseSFXtime{ 5.5f };
	bool m_DoorCloseSFXplayed{ false };


	// AUDIO
	FMOD::Channel* m_pDoorChannel{};
	FMOD::Sound* m_pDoorAnimSound{};
};

